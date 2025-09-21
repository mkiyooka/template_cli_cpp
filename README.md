# C++ Template CLI - メインアプリケーション

このプロジェクトは、モダンC++を使用したCLIアプリケーションのテンプレートです。軽量で拡張可能なCLIアプリケーションの基盤を提供します。

## プロジェクト構成

```text
- src/                          # メインアプリケーションのソースコード
    - main.cpp                  # エントリーポイント
    - command/                  # サブコマンド実装
        - cli.cpp               # CLI メイン処理
        - subcommand_add.cpp    # 加算サブコマンド
        - subcommand_mul.cpp    # 乗算サブコマンド
    - config/                   # 設定管理
        - config_loader.cpp     # TOML設定読み込み
- include/                      # ヘッダーファイル
    - command/                  # コマンド関連ヘッダー
    - config/                   # 設定関連ヘッダー
    - utility/                  # ユーティリティ
- tests/                        # テストコード
    - cpp/                      # C++テスト
        - test_toml.cpp         # TOML設定テスト
- cmake/                        # CMake設定ファイル
- example.toml                  # 設定ファイルサンプル
- CMakeLists.txt                # ビルド設定
- conanfile.py                  # 依存関係定義
- README.md                     # プロジェクト説明
```

## クイックスタート

### 必要な環境

- **CMake** 3.19以上
- **Conan** 2.0以上（パッケージ管理）
- **C++17**対応コンパイラ

### ビルド手順

```bash
# 1. 依存関係のインストール
conan install . --output-folder=build --build=missing

# 2. CMake設定（デフォルトでReleaseビルド）
cmake -S . -B build

# 3. ビルド
cmake --build build

# 4. メインアプリケーション実行
./build/src/cmd --help
./build/src/cmd add 10 20    # 出力: 10 + 20 = 30
./build/src/cmd mul 5 7     # 出力: 5 * 7 = 35
```

### 他のビルドタイプを使用する場合

```bash
# デバッグビルド
cmake --preset debug
cmake --build build

# リリースビルド（明示的に指定）
cmake --preset release
cmake --build build

# リリース+デバッグ情報
cmake --preset relwithdebinfo
cmake --build build
```

## コア機能

### メインCLIアプリケーション

- **add**: 2つの数値の加算計算
- **mul**: 2つの数値の乗算計算
- **設定ファイル対応**: TOML形式の設定ファイル読み込み

### 使用しているライブラリ

| 技術             | 用途                   | 理由                          |
| ---------------- | ---------------------- | ----------------------------- |
| **CLI11**        | コマンドライン引数解析 | 直感的なAPI、サブコマンド対応 |
| **tomlplusplus** | 設定ファイル解析       | 高性能なモダンTOMLライブラリ  |
| **fmt**          | 文字列フォーマット     | 高性能で安全な文字列操作      |
| **doctest**      | テストフレームワーク   | 軽量でヘッダーオンリー        |

## 開発ツール

### コード品質管理

```bash
# フォーマット
cmake --build build --target format
# make -C build format

# リント
cmake --build build --target lint
# make -C build lint

# 全品質チェック
cmake --build build --target fullcheck
# make -C build fullcheck

# テスト実行
cmake --build build --target run-tests
# make -C build run-tests

# 個別テスト実行
./build/tests/cpp/test_toml

# 利用可能なターゲット表示
cmake --build build --target show-help
```

### コード品質管理に利用するツール

- **clang-format**: コードフォーマット
- **clang-tidy**: 静的解析とリント
- **cppcheck**: 追加の静的解析
- **doctest**: ユニットテスト

## 拡張方法

### 新しいサブコマンドの追加

1. `include/command/subcommand_*.hpp` にヘッダーを作成
2. `src/command/subcommand_*.cpp` に実装を追加
3. `src/command/cli.cpp` でサブコマンドを登録

### 設定項目の追加

1. `include/config/` で設定構造体を定義
2. TOMLファイルに新しい設定項目を追加
3. 設定読み込み処理を更新

## 設定ファイルの使用

### TOML設定ファイル

プロジェクトには設定ファイルサンプル `example.toml` が含まれています：

```toml
title = "Example TOML Configuration"
value = 42
```

### 設定ファイルの使用方法

```bash
# 設定ファイルを指定してアプリケーションを実行
./build/src/cmd --config example.toml add 10 20

# 設定ファイルの内容確認
cat example.toml
```

設定ファイルの内容は実行時に読み込まれ、`title` と `value` の値がコンソールに出力されます。
