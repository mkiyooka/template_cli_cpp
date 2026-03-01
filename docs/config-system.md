# 設定システム設計ドキュメント

## 概要

このプロジェクトの設定システムは、CLI引数と設定ファイル（TOML / JSONC / YAML）を統合管理する。
新しいオプションを追加する際の記述箇所を最小化することを目的として設計されている。

## アーキテクチャ

```text
┌──────────────────────────────────────────────────────┐
│  config_schema.hpp                                   │
│  FieldDescriptor + kConfigSchema                     │
│  (CLIオプション名・設定キー・デフォルト値の一元定義)      │
└──────────────┬──────────────────┬────────────────────┘
               │                  │
       ┌───────▼──────┐  ┌────────▼───────────┐
       │ ConfigManager│  │ ConfigFileLoader   │
       │ (CLI11登録・  │  │ (TOML/JSON/YAML    │
       │  優先度解決)  │  │  読み込み)          │
       └───────┬──────┘  └────────────────────┘
               │
       ┌───────▼──────┐
       │    Config    │
       │ (解決済み値)  │
       └──────────────┘
```

## ファイル構成

- `include/config/`
    - `config_loader.hpp` — `Config` / `PluginConfig` 構造体定義
    - `config_schema.hpp` — `FieldDescriptor` テンプレートと `kConfigSchema` 定義
    - `config_file_loader.hpp` — `LoadFromFile` / `FindDefaultConfig` 宣言
    - `config_manager.hpp` — `ConfigManager` クラス宣言・`ShowConfig` 宣言
- `src/config/`
    - `config_loader.cpp` — （構造体定義のみ、実装は他ファイルに移管）
    - `config_file_loader.cpp` — TOML / JSONC / YAML 読み込み実装
    - `config_manager.cpp` — `ConfigManager` 実装・`ShowConfig` 実装

## 優先度

```text
CLI引数 > 設定ファイル > Config構造体のデフォルト値
```

## 設定ファイル対応形式

|拡張子|形式|ライブラリ|備考|
|---|---|---|---|
|`.toml`|TOML|tomlplusplus v3.4.0||
|`.json`|JSONC|nlohmann/json v3.12.0|`//` コメント対応|
|`.yaml` / `.yml`|YAML|fkYAML v0.4.2||

## CLIオプション名の規則

ネストはドット区切りで表現する。

```sh
./build/cmd_cli11 --title="MyApp" --settings.value=42
```

設定ファイルのキー構造と対応する。

```toml
title = "MyApp"

[settings]
value = 42
```

## デフォルト設定ファイルの自動探索

`-c / --config` を指定しない場合、以下のパスを順に探索する。

- `config/default.toml`
- `config/default.json`
- `config/default.yaml`

複数のファイルが同時に存在する場合はエラーとなる。

```text
Error: Multiple default config files found: config/default.toml config/default.json
```

## 新しいオプションの追加方法

変更箇所は **2ファイルのみ**。

### 1. `include/config/config_loader.hpp` にフィールドを追加

```cpp
struct Config {
    std::string title = "title";
    std::uint64_t value = 10;
    std::string log_level = "info";  // 追加
    std::vector<PluginConfig> plugins;
};
```

### 2. `include/config/config_schema.hpp` の `kConfigSchema` に1行追加

```cpp
inline constexpr auto kConfigSchema = std::make_tuple(
    FieldDescriptor{"--title",          "title",          "Application title", &Config::title},
    FieldDescriptor{"--settings.value", "settings.value", "Numeric value",     &Config::value},
    FieldDescriptor{"--logging.level",  "logging.level",  "Log level",         &Config::log_level}  // 追加
);
```

これだけで以下が自動的に有効になる。

- CLI11 への `--logging.level` オプション登録
- 設定ファイルの `logging.level` キーからの読み込み（TOML / JSONC / YAML）
- `ShowConfig` での表示

### 対応する設定ファイル例（TOML）

```toml
[logging]
level = "warn"
```

### 対応する設定ファイル例（JSONC）

```json
{
    // ログ設定
    "logging": {
        "level": "warn"
    }
}
```

### 対応する設定ファイル例（YAML）

```yaml
logging:
  level: warn
```

## スキーマが自動化する処理

`kConfigSchema` の各エントリは `std::tuple` に格納されており、
`std::apply` によりコンパイル時に展開される。

### CLI11 登録（`ConfigManager::RegisterOptions`）

```cpp
std::apply([&](auto&&... field) {
    ([&] {
        auto* opt = app.add_option(
            std::string(field.cli_option),
            cli_values_.*field.member,
            std::string(field.description)
        );
        // CLI指定フラグを記録
        opt->each([this, i](const std::string&) { cli_set_[i] = true; });
    }(), ...);
}, kConfigSchema);
```

### 設定ファイル読み込み（`config_file_loader.cpp` 内）

ドット区切りの `config_key` を再帰的に分解してネストテーブルを辿る。

```text
"settings.value" → tbl["settings"]["value"]
```

### 優先度解決（`ConfigManager::Resolve`）

```text
1. Config result{};            // デフォルト値で初期化
2. LoadFromFile(...);          // ファイル値で上書き
3. cli_set_[i] が true の場合  // CLI値で上書き
```

## plugins フィールドの扱い

`std::vector<PluginConfig>` のような複合型はスキーマ管理の対象外。
CLIからの指定はできず、設定ファイル専用フィールドとして
`config_file_loader.cpp` 内で個別に実装する。

```toml
[[plugin]]
file = "a.so"
number = 1
```

## `ShowConfig` の出力形式

スキーマフィールドは `config_key: 値` の形式で出力される。
新しいフィールドをスキーマに追加すると自動的に出力に含まれる。

```text
title: MyApp
settings.value: 42
plugin: file=a.so, number=1
```
