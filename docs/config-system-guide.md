# 設定システム利用ガイド

設計・実装の詳細は [config-system.md](config-system.md) を参照。

---

## 基本的な使い方

### 1. Config 構造体にフィールドを追加する

`include/config/config_loader.hpp` にフィールドとデフォルト値を定義する。

```cpp
struct Config {
    std::string title   = "title";
    std::uint64_t value = 10;
    std::string log_level = "info";  // 追加
};
```

### 2. スキーマに1行追加する

`include/config/config_schema.hpp` の `kConfigSchema` に対応エントリを追加する。

```cpp
inline constexpr auto kConfigSchema = std::make_tuple(
    FieldDescriptor{"--title",          "title",          "Application title", &Config::title},
    FieldDescriptor{"--settings.value", "settings.value", "Numeric value",     &Config::value},
    FieldDescriptor{"--logging.level",  "logging.level",  "Log level",         &Config::log_level}  // 追加
);
```

これだけで以下が自動的に有効になる。

- CLI11 への `--logging.level` オプション登録
- TOML / JSONC / YAML の `logging.level` キーからの読み込み
- `cli.cpp` の `ShowConfig` での表示（`kConfigSchema` をループするため自動対応）

### 3. アプリに組み込む

```cpp
#include "config/config_manager.hpp"

CLI::App app{"My App"};

std::string config_file;
app.add_option("-c,--config", config_file, "Configuration file");

config::ConfigManager mgr;
mgr.RegisterOptions(app);   // スキーマ由来の全オプションを登録

app.parse(argc, argv);

// スキーマフィールドを解決（CLI引数 > 設定ファイル > デフォルト値）
Config conf = mgr.Resolve(config_file);

// スキーマ外フィールド（plugins 等）はファイルの生値から取得
const Config &file_vals = mgr.GetFileValues();
conf.plugins = file_vals.plugins;
```

### 4. Config の値を使う

```cpp
fmt::print("title: {}\n", conf.title);
fmt::print("value: {}\n", conf.value);

for (const auto& p : conf.plugins) {
    fmt::print("plugin: {} ({})\n", p.file, p.number);
}
```

---

## 設定ファイルの書き方

### TOML（推奨）

```toml
title = "My Application"

[settings]
value = 42

[logging]
level = "warn"

[[plugin]]
file = "a.so"
number = 1
```

### JSONC

```json
{
    "title": "My Application",
    "settings": {
        "value": 42
    },
    "logging": {
        "level": "warn"
    }
}
```

### YAML

```yaml
title: My Application
settings:
    value: 42
logging:
    level: warn
```

---

## CLI からの実行例

```sh
# デフォルト設定ファイル（config/default.toml 等）を自動探索
./build/cmd_cli11

# 設定ファイルを明示指定
./build/cmd_cli11 -c config/example.toml

# CLI 引数で設定ファイルの値を上書き（優先度: CLI > ファイル > デフォルト）
./build/cmd_cli11 -c config/example.toml --title="Override Title" --settings.value=99

# 設定ファイルなしで CLI 引数のみ使用
./build/cmd_cli11 --title="CLI Only"

# 現在の全設定値を確認する
./build/cmd_cli11 -c config/example.toml
```

---

## 設定ファイルの自動探索

`-c / --config` を省略した場合、以下のパスを順に探索する。

- `config/default.toml`
- `config/default.json`
- `config/default.yaml`

複数のファイルが同時に存在する場合はエラーになる。

```text
Error: Multiple default config files found: config/default.toml config/default.json
```

---

## plugins フィールドの扱い

`std::vector<PluginConfig>` はスキーマ管理の対象外で、設定ファイル専用フィールド。
CLI からの指定には対応していない。

```cpp
struct PluginConfig {
    std::string file;
    std::uint64_t number;
};
```

```toml
[[plugin]]
file = "a.so"
number = 1

[[plugin]]
file = "b.so"
number = 2
```

---

## サブコマンド別設定

各サブコマンド（`add`, `subtract`, `multiply`, `divide`）には、
設定ファイルでデフォルトのオペランドを指定できる。

### TOML

```toml
[subcommands.add]
a = 10
b = 5

[subcommands.multiply]
a = 3
b = 7
```

### JSONC

```json
{
    "subcommands": {
        "add":      { "a": 10, "b": 5 },
        "multiply": { "a": 3,  "b": 7 }
    }
}
```

### YAML

```yaml
subcommands:
  add:
    a: 10
    b: 5
  multiply:
    a: 3
    b: 7
```

### 優先度

```text
CLI 引数 > 設定ファイルの subcommands 値 > デフォルト値 (0)
```

サブコマンドを CLI で指定した場合（例: `./build/cmd add 3 4`）、
CLI の値が使われ、設定ファイルの値は無視される。
サブコマンドが指定されなかった場合は設定ファイルの値が使われる。
