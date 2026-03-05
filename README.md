# C++ CLI Template

C++17 CLI アプリケーションのテンプレートプロジェクト。
CLI11 を使ったサブコマンド構成と、TOML / JSON / YAML 対応の設定システムを備える。
pixi による再現性の高い開発環境と、カバレッジ計測・コード品質ツールを提供する。

## 必要条件

- [pixi](https://prefix.dev/docs/pixi/overview)

pixi をインストール後、`pixi install` でコンパイラ・ツール類が自動セットアップされる。

## クイックスタート

```bash
pixi run config   # CMake 設定（Release）
pixi run build    # ビルド
pixi run test     # テスト実行

# アプリ実行例
./build/template_cli_cpp add 10 20
./build/template_cli_cpp --config config/example.toml subtract 15 5
```

## 対応プラットフォーム

| プラットフォーム              | コンパイラ        | リンカ             |
| ----------------------------- | ----------------- | ------------------ |
| Linux x86-64                  | GCC 15 / Clang 21 | mold（高速リンク） |
| macOS (Apple Silicon / Intel) | AppleClang        | system             |

## 導入ライブラリ

### アプリケーション

| ライブラリ    | バージョン | 用途                          |
| ------------- | ---------- | ----------------------------- |
| CLI11         | 2.5.0      | コマンドライン引数解析        |
| fmt           | 12.0.0     | 文字列フォーマット            |
| toml++        | 3.4.0      | TOML 設定ファイル解析         |
| nlohmann/json | 3.12.0     | JSON / JSONC 設定ファイル解析 |
| yyjson        | 0.12.0     | 高速 JSON 読み書き            |
| fkYAML        | 0.4.2      | YAML 設定ファイル解析         |
| spdlog        | 1.17.0     | ロギング                      |

### テスト・ベンチマーク

| ライブラリ | バージョン | 用途                 |
| ---------- | ---------- | -------------------- |
| doctest    | 2.4.11     | テストフレームワーク |
| nanobench  | 4.3.11     | マイクロベンチマーク |

## 主要タスク一覧

```bash
pixi run config           # CMake 設定（Release）
pixi run config-debug     # CMake 設定（Debug）
pixi run build            # ビルド
pixi run test             # テスト実行
pixi run format           # clang-format によるコード整形
pixi run lint             # clang-tidy による静的解析
pixi run run-cppcheck     # cppcheck による静的解析
pixi run fullcheck        # typos + lint + cppcheck をまとめて実行

# カバレッジ（macOS / Linux 共通）
pixi run config-coverage  # カバレッジビルドの設定
pixi run coverage         # 計測・テキスト & HTML レポート生成
pixi run coverage-report  # HTML レポートをブラウザで開く（macOS のみ）
```

詳細なビルドシステムの説明は [docs/build-system.md](docs/build-system.md) を参照。

## 設定システム

CLI 引数・設定ファイル・デフォルト値を統合管理する。

**優先度**: CLI 引数 > 設定ファイル > デフォルト値

対応フォーマット:

| 拡張子           | 形式  | 備考              |
| ---------------- | ----- | ----------------- |
| `.toml`          | TOML  |                   |
| `.json`          | JSONC | `//` コメント対応 |
| `.yaml` / `.yml` | YAML  |                   |

```bash
# 設定ファイルを指定して実行
./build/template_cli_cpp --config config/example.toml add 10 20

# CLI 引数で設定値を上書き（ファイルより優先）
./build/template_cli_cpp --config config/example.toml --settings.value=99 add 10 20
```

`--config` を省略した場合、`config/default.{toml,json,yaml}` を自動探索する（複数存在はエラー）。

詳細は [docs/config-system.md](docs/config-system.md) および [docs/config-system-guide.md](docs/config-system-guide.md) を参照。

## ディレクトリ構成

- `src/` — アプリケーションソースコード
    - `command/` — CLI エントリポイント・サブコマンド
    - `config/` — 設定ファイルの読み込み・管理
    - `sut_example/` — サンプル実装
- `include/` — ヘッダファイル
    - `command/` — CLI インターフェース
    - `config/` — 設定システム
    - `template_cli_cpp/` — ライブラリ群（logging, output, recording）
    - `utility/` — ユーティリティ（yyjson ラッパーなど）
- `tests/` — テストコード（doctest）
- `benches/` — ベンチマーク（nanobench）
- `config/` — 設定ファイルサンプル（TOML / JSON / YAML）
- `cmake/` — CMake モジュール群
- `docs/` — ドキュメント

## ドキュメント

- [docs/build-system.md](docs/build-system.md) — ビルドシステム・開発ツールの詳細
- [docs/config-system.md](docs/config-system.md) — 設定システムの仕様
- [docs/config-system-guide.md](docs/config-system-guide.md) — 設定システムの拡張ガイド
- [docs/output-system.md](docs/output-system.md) — 出力システムの仕様
- [docs/output-system-guide.md](docs/output-system-guide.md) — 出力システムの拡張ガイド
