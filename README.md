# C++ CLI Library Examples

C++のCLIライブラリの使用例を集めたサンプルコード集です。CMake FetchContentを使用してライブラリを管理しています。

## プロジェクト構成

- **ビルドシステム**: CMake 3.19 + Ninja
- **環境管理**: Pixi (クロスプラットフォーム対応)
- **テストフレームワーク**: doctest
- **C++標準**: C++17

## 導入ライブラリ

- **CLI11**: コマンドライン引数解析
- **morrisfranken argparse**: 軽量コマンドライン引数解析
- **fmt**: 文字列フォーマット
- **tomlplusplus**: TOML設定ファイル解析
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
# CLI11版
./build/src/cmd_cli11 add 10 20        # 10 + 20 = 30
./build/src/cmd_cli11 subtract 15 5    # 15 - 5 = 10

# Morris argparse版
./build/src/cmd_argparse_morris add --a 10 --b 20        # 10 + 20 = 30
./build/src/cmd_argparse_morris subtract --a 15 --b 5    # 15 - 5 = 10
```

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

- `src/`: サンプルアプリケーション
- `include/`: ヘッダーファイル
- `tests/`: テストコード
- `cmake/`: CMake設定ファイル
    - `local-or-fetch.cmake`: FetchContentヘルパー
    - `dependencies-app.cmake`: アプリ用ライブラリ
    - `dependencies-test.cmake`: テスト用ライブラリ
    - `custom-targets.cmake`: カスタムターゲット
    - `quality-setup.cmake`: コード品質設定
    - `quality-tools.cmake`: コード品質ツール
- `third_party/`: サードパーティライブラリ (FetchContentでダウンロード)
