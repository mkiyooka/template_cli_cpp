# 設定システム設計ドキュメント

利用ガイドは [config-system-guide.md](config-system-guide.md) を参照。

---

## 概要

このプロジェクトの設定システムは、**CLI引数**と**設定ファイル（TOML / JSONC / YAML）**を統合管理する。

設計上の最重要目標は「**オプションを1つ追加するときの変更箇所を2ファイル・最小行数に抑える**」ことである。
これにより、機能追加時の記述漏れや、CLI・設定ファイル間での動作の不一致を防ぐ。

### 対応する設定ファイル形式

| 拡張子           | 形式  | ライブラリ            | 備考              |
| ---------------- | ----- | --------------------- | ----------------- |
| `.toml`          | TOML  | tomlplusplus v3.4.0   |                   |
| `.json`          | JSONC | nlohmann/json v3.12.0 | `//` コメント対応 |
| `.yaml` / `.yml` | YAML  | fkYAML v0.4.2         |                   |

### 優先度

```text
CLI引数 > 設定ファイル > Config構造体のデフォルト値
```

---

## 概要設計

### コンポーネント構成

```mermaid
graph TD
    Schema["config_schema.hpp\nFieldDescriptor + kConfigSchema\n（CLIオプション名・設定キー・メンバーポインタの一元定義）"]
    Manager["ConfigManager\n（CLI11登録・優先度解決）"]
    FileLoader["ConfigFileLoader\n（TOML / JSONC / YAML 読み込み）"]
    Config["Config\n（解決済み値）"]
    CLI11["CLI11::App\n（コマンドライン引数解析）"]
    Files["設定ファイル\n(.toml / .json / .yaml)"]

    Schema --> Manager
    Schema --> FileLoader
    Manager --> Config
    FileLoader --> Config
    CLI11 --> Manager
    Files --> FileLoader
```

### 起動時の処理フロー

```mermaid
sequenceDiagram
    participant main as main()
    participant mgr as ConfigManager
    participant app as CLI11::App
    participant loader as ConfigFileLoader
    participant conf as Config

    main->>mgr: RegisterOptions(app)
    Note over mgr,app: スキーマを展開してCLI11にオプションを一括登録
    main->>app: parse(argc, argv)
    main->>mgr: Resolve(config_path)
    mgr->>conf: デフォルト値で初期化
    mgr->>loader: FindDefaultConfig() / LoadFromFile()
    loader->>conf: ファイル値で上書き
    mgr->>conf: CLI値で上書き（指定されたフィールドのみ）
    mgr-->>main: Config（解決済み）
```

### 設定ファイルの自動探索

`-c / --config` を指定しない場合、以下のパスを順に探索する。

- `config/default.toml`
- `config/default.json`
- `config/default.yaml`

複数のファイルが同時に存在する場合はエラーとなる。

```text
Error: Multiple default config files found: config/default.toml config/default.json
```

`--config` を明示指定した場合は自動探索をスキップし、他形式のデフォルトファイルが存在しても無視する。

### CLIオプション名の規則

設定ファイルのネスト構造をドット区切りで表現する。

```sh
# ファイルの settings.value キーに対応するCLIオプション
./build/cmd_cli11 --settings.value=42
```

---

## C++ 詳細設計

### ファイル構成

```mermaid
graph LR
    subgraph include/config/
        A["config_loader.hpp\nConfig / PluginConfig / SubcommandMapping 構造体"]
        B["config_schema.hpp\nFieldDescriptor / kConfigSchema"]
        C["config_file_loader.hpp\nLoadFromFile / FindDefaultConfig"]
        D["config_manager.hpp\nConfigManager"]
    end
    subgraph src/config/
        E["config_loader.cpp\n（スタブ）"]
        F["config_file_loader.cpp\nTOML / JSONC / YAML 実装"]
        G["config_manager.cpp\nConfigManager 実装"]
    end
    subgraph src/command/
        H["subcommand.cpp\nkSubcommandMappings 実体定義"]
    end
    B --> F
    B --> G
    A --> B
    C --> G
    A --> H
```

### FieldDescriptor テンプレート

`Config` 構造体の1フィールドと、CLIオプション名・設定ファイルキーを紐づける記述子。

```cpp
// include/config/config_schema.hpp

template <typename Owner, typename T>
struct FieldDescriptor {
    std::string_view cli_option;  // "--settings.value"
    std::string_view config_key;  // "settings.value"（ドット区切りでネスト表現）
    std::string_view description; // CLIヘルプ文字列
    T Owner::*member;             // &Config::value（ポインタ・トゥ・メンバー）
};

// C++17 CTAD補助：型引数を省略して記述できるようにする
template <typename Owner, typename T>
FieldDescriptor(std::string_view, std::string_view, std::string_view, T Owner::*)
    -> FieldDescriptor<Owner, T>;
```

**ポインタ・トゥ・メンバー**（`T Owner::*member`）を使うことで、
型安全にフィールドへアクセスできる。アクセス時は `conf.*field.member` と記述する。

### kConfigSchema の定義

全オプションを `std::tuple` に格納する。
`std::tuple` を使うことで、各フィールドの型を静的に保持でき、
後述の `std::apply` によりコンパイル時に展開される。

```cpp
// include/config/config_schema.hpp

inline constexpr auto kConfigSchema = std::make_tuple(
    FieldDescriptor{"--title",          "title",          "Application title", &Config::title},
    FieldDescriptor{"--settings.value", "settings.value", "Numeric value",     &Config::value}
);
```

### ConfigManager クラス

```cpp
// include/config/config_manager.hpp

class ConfigManager {
public:
    void RegisterOptions(CLI::App& app);
    Config Resolve(const std::string& explicit_config_path); // スキーマフィールドのみ解決
    const Config& GetFileValues() const;                     // ファイルの生値（スキーマ外フィールド取得用）
private:
    Config cli_values_;         // CLI11のパース結果書き込み先
    Config file_values_;        // 設定ファイルから読み込んだ値（Resolve() 後に有効）
    std::vector<bool> cli_set_; // 各フィールドがCLIで明示指定されたか
};
```

`Resolve()` はスキーマ定義フィールド（`kConfigSchema` に列挙されたもの）のみを解決して返す。
`plugins` や `SubcommandConfig` などスキーマ外の複合型フィールドは `GetFileValues()` で取得し、
呼び出し元（`cli.cpp`）でマージする。

`cli_values_` はスキーマサイズ分のフィールドを持つ `Config`。
`cli_set_` はどのフィールドが実際にCLIで指定されたかを記録するフラグ配列。
この2つを分離することで「CLIで未指定のフィールドはファイル値を使う」という優先度を実現する。

---

## 実装の工夫

### std::apply によるコンパイル時タプル展開

`kConfigSchema` のループ処理には `std::apply`（C++17）を使う。
これにより、タプルの各要素に対してテンプレートクロージャを展開でき、
**型情報を失わずに**全フィールドを一括処理できる。

#### CLI11 への一括登録（RegisterOptions）

```cpp
std::apply(
    [&](auto &&...field) {
        ([&] {
            auto *opt = app.add_option(
                std::string(field.cli_option),
                cli_values_.*field.member,  // パース結果の書き込み先
                std::string(field.description)
            );
            const std::size_t i = idx++;
            // パース後コールバックでCLI指定フラグを立てる
            opt->each([this, i](const std::string &) { cli_set_[i] = true; });
        }(), ...);  // 即時呼び出し展開（fold expression）
    },
    kConfigSchema
);
```

`opt->each(callback)` はそのオプションが実際にパースされたときに呼ばれる。
これにより「ユーザーが明示的に指定したか」を `cli_set_[i]` に記録できる。
デフォルト値と `0` や空文字列が区別できない場合でも正確に判定できる。

#### 設定ファイル読み込みへの適用

`std::remove_reference_t<decltype(conf.*field.member)>` でメンバーの型を取得し、
各パーサーの型付きAPIに渡す。これにより型変換コードを書かなくてよい。

```cpp
std::apply(
    [&](auto &&...field) {
        ([&] {
            // メンバー型をコンパイル時に取得
            using FieldType = std::remove_reference_t<decltype(conf.*field.member)>;
            auto val = ResolveTomlKey<FieldType>(tbl, field.config_key);
            if (val.has_value()) {
                conf.*field.member = *val;  // ファイルに存在するキーのみ上書き
            }
        }(), ...);
    },
    kConfigSchema
);
```

#### 設定表示（ShowConfig）への適用

スキーマを参照するため、新しいフィールドを追加すると表示も自動的に増える。
この関数はユーザコードの `cli.cpp` に定義する（`config_manager` の責務外）。

```cpp
std::apply(
    [&](auto &&...field) {
        ([&] { fmt::print("{}: {}\n", field.config_key, conf.*field.member); }(), ...);
    },
    config::kConfigSchema
);
```

### ドット区切りキーの再帰解決

設定ファイルのネストを `"settings.value"` のようなドット区切り文字列で表現し、
再帰関数でノードを辿る。TOML / JSON / YAML の各パーサーで同じパターンを実装している。

```cpp
// TOML の例（JSON・YAML も同様のパターン）
template <typename T>
std::optional<T> ResolveTomlKey(const toml::table &tbl, std::string_view dotted_key) {
    const auto dot_pos = dotted_key.find('.');
    if (dot_pos == std::string_view::npos) {
        // リーフノードに到達 → 値を取得
        return tbl[dotted_key].template value<T>();
    }
    // ドットで分割して再帰
    const auto head = dotted_key.substr(0, dot_pos); // "settings"
    const auto tail = dotted_key.substr(dot_pos + 1); // "value"
    const auto *nested = tbl[head].as_table();
    if (nested == nullptr) {
        return std::nullopt; // キーが存在しない → スキップ
    }
    return ResolveTomlKey<T>(*nested, tail);
}
```

戻り値に `std::optional<T>` を使うことで「キーが存在しない」と「値が0や空文字列」を区別し、
**ファイルに存在するキーのみ上書き**するという仕様を安全に実装できる。

### 優先度解決（Resolve の実装）

```mermaid
flowchart TD
    A[Config result をデフォルト値で初期化] --> B{config_path は空か？}
    B -- No --> C[LoadFromFile でファイル値を読み込み]
    B -- Yes --> D[FindDefaultConfig でデフォルトファイルを探索]
    D --> E{ファイルが見つかったか？}
    E -- Yes --> C
    E -- No --> F[スキーマ全フィールドをファイル値で上書き]
    C --> F
    F --> G{各フィールドについて cli_set_ が true か？}
    G -- Yes --> H[CLI値で上書き]
    G -- No --> I[ファイル値・デフォルト値を維持]
    H --> J[Config を返す]
    I --> J
```

### plugins フィールドの扱い

`std::vector<PluginConfig>` のような複合型はスキーマ管理の対象外とし、
`config_file_loader.cpp` 内で個別に実装する。
CLIからの指定は対応せず、設定ファイル専用フィールドとして扱う。

```toml
# TOML での記述例
[[plugin]]
file = "a.so"
number = 1

[[plugin]]
file = "b.so"
number = 2
```

### サブコマンド別設定フィールドの扱い

`SubcommandConfig` のような複合型はスキーマ管理の対象外。
サブコマンド名と `Config` メンバーのマッピングは `SubcommandMapping` 構造体と
`kSubcommandMappings` 配列で管理し、`src/command/subcommand.cpp` で実体を定義する。
サブコマンドを追加・変更する際は `subcommand.cpp` のみ修正すればよい。

優先度解決の仕組み:

1. CLI11 がサブコマンド引数を `Config` の `add.a`, `add.b` 等に直接書き込む
2. `ConfigManager::Resolve()` が設定ファイルを読み込み、`GetFileValues()` でアクセス可能にする
3. `cli.cpp` が `kSubcommandMappings` をループし、CLI 未指定のサブコマンドにファイル値を適用する

```cpp
// cli.cpp でのマージ例
const Config &file_vals = config_manager.GetFileValues();
for (std::size_t i = 0; i < kSubcommandMappingCount; ++i) {
    const auto &m = kSubcommandMappings[i];
    if (!app.got_subcommand(m.key)) {
        config.*m.member = file_vals.*m.member;
    }
}
```

```toml
# TOML での記述例
[subcommands.add]
a = 10
b = 5
```
