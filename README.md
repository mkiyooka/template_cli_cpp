# C++ Template CLI

C++のCLIライブラリの使用例を集めたテンプレートプロジェクトです。CMake FetchContentを使用してライブラリを管理しています。

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

## ディレクトリ構造

- `src/`: メインアプリケーション
- `include/`: ヘッダーファイル
- `tests/cpp/`: テストコード
- `config_example/`: 設定ファイルサンプル
