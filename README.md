# C++ CLI Template

C++でCLI11を使ったアプリケーションを実装するためのテンプレートです。
CLIオプションと設定ファイル（TOML / JSONC / YAML）の統合管理機能を備えています。

## プロジェクト構成

- **ビルドシステム**: CMake 3.19 + Ninja
- **環境管理**: Pixi (クロスプラットフォーム対応)
- **テストフレームワーク**: doctest
- **C++標準**: C++17

## 導入ライブラリ

- **CLI11**: コマンドライン引数解析
- **fmt**: 文字列フォーマット
- **tomlplusplus**: TOML設定ファイル解析
- **nlohmann/json**: JSON / JSONC設定ファイル解析
- **fkYAML**: YAML設定ファイル解析
- **doctest**: テストフレームワーク

## セットアップ

```bash
# Pixi環境のインストール
pixi install

# CMake設定とビルド
pixi run config
pixi run build

# テスト実行
pixi run test
```

## サンプル実行例

```bash
# 基本的なサブコマンド
./build/cmd_cli11 add 10 20        # 10 + 20 = 30
./build/cmd_cli11 subtract 15 5    # 15 - 5 = 10

# 設定ファイルを指定して実行
./build/cmd_cli11 --config config/example.toml add 10 20
./build/cmd_cli11 --config config/example.json add 10 20
./build/cmd_cli11 --config config/example.yaml add 10 20

# CLIオプションで設定値を指定（ファイルより優先）
./build/cmd_cli11 --config config/example.toml --settings.value=99 add 10 20
```

## 設定システム

CLIオプションと設定ファイルを統合管理する仕組みを実装しています。

### 優先度

```text
CLI引数 > 設定ファイル > デフォルト値
```

### 対応する設定ファイル形式

|拡張子|形式|備考|
|---|---|---|
|`.toml`|TOML||
|`.json`|JSONC|`//` コメント対応|
|`.yaml` / `.yml`|YAML||

### デフォルト設定ファイルの自動探索

`--config` を指定しない場合、以下のいずれかを自動で読み込む（複数存在はエラー）。

- `config/default.toml`
- `config/default.json`
- `config/default.yaml`

### 新しいオプションの追加

変更箇所は2ファイルのみ。

1. `include/config/config_loader.hpp` にフィールドを追加
2. `include/config/config_schema.hpp` の `kConfigSchema` に1行追加

```cpp
// include/config/config_schema.hpp
inline constexpr auto kConfigSchema = std::make_tuple(
    FieldDescriptor{"--title",          "title",          "Application title", &Config::title},
    FieldDescriptor{"--settings.value", "settings.value", "Numeric value",     &Config::value},
    FieldDescriptor{"--logging.level",  "logging.level",  "Log level",         &Config::log_level}  // 追加
);
```

詳細は [docs/config-system.md](docs/config-system.md) を参照。

## 開発ツール

```bash
# コードフォーマット
pixi run format

# 静的解析
pixi run lint

# 全チェック実行
pixi run fullcheck
```

## ディレクトリ構成

- `src/` — サンプルアプリケーション
- `include/` — ヘッダーファイル
- `tests/` — テストコード
- `config/` — サンプル設定ファイル（TOML / JSONC / YAML）
- `docs/` — ドキュメント
- `cmake/` — CMake設定ファイル
    - `local-or-fetch.cmake` — FetchContentヘルパー
    - `dependencies-app.cmake` — アプリ用ライブラリ
    - `dependencies-test.cmake` — テスト用ライブラリ
    - `custom-targets.cmake` — カスタムターゲット
    - `quality-setup.cmake` — コード品質設定
    - `quality-tools.cmake` — コード品質ツール
- `third_party/` — サードパーティライブラリ (FetchContentでダウンロード)
