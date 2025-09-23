# C++ Template CLI - 複数CLIライブラリ対応

このプロジェクトは、モダンC++を使用したCLIアプリケーションのテンプレートです。複数のCLIライブラリの実装を比較・学習できる構成になっています。

## プロジェクト構成

```text
- src/                              # メインアプリケーションのソースコード
    - main_cli11.cpp                # CLI11版エントリーポイント
    - main_argparse_morris.cpp      # Morris argparse版エントリーポイント
    - command/                      # サブコマンド実装
        - cli_cli11.cpp             # CLI11メイン処理
        - subcommand_cli11.cpp      # CLI11サブコマンド実装
        - cli_argparse_morris.cpp   # Morris argparseメイン処理
        - subcommand_argparse_morris.cpp # Morris argparseサブコマンド実装
    - config/                       # 設定管理
        - config_loader.cpp         # TOML設定読み込み
- include/                          # ヘッダーファイル
    - command/                      # コマンド関連ヘッダー
    - config/                       # 設定関連ヘッダー
- tests/                            # テストコード
    - cpp/                          # C++テスト
        - test_toml.cpp             # TOML設定テスト
- cmake/                            # CMake設定ファイル
- config.toml                       # 設定ファイルサンプル
- CMakeLists.txt                    # ビルド設定
- conanfile.py                      # 依存関係定義
- README.md                         # プロジェクト説明
```

## 実装されているCLIライブラリ

| ライブラリ | 実行ファイル | 特徴 | 使用例 |
|------------|-------------|------|--------|
| **CLI11** | `cmd_cli11` | 機能豊富、callback/got_subcommand両方式対応 | `./build/src/cmd_cli11 add 5 3` |
| **morrisfranken argparse** | `cmd_argparse_morris` | 軽量・高速、構造体継承方式 | `./build/src/cmd_argparse_morris add --a 5 --b 3` |

## クイックスタート

### 必要な環境

- **CMake** 3.19以上
- **Conan** 2.0以上（パッケージ管理）
- **C++17**対応コンパイラ

### ビルド手順

```bash
# 1. 依存関係のインストール
conan install . --output-folder=build --build=missing

# 2. CMake設定（リリースビルド）
cmake --preset release

# 3. ビルド
cmake --build build

# 4. 各実行ファイルの使用方法
./build/src/cmd_cli11 --help
./build/src/cmd_argparse_morris --help
```

### 各ライブラリの使用例

#### CLI11版

```bash
# CLI11版の使用例（位置引数）
./build/src/cmd_cli11 add 10 20        # 10 + 20 = 30
./build/src/cmd_cli11 subtract 15 5    # 15 - 5 = 10
./build/src/cmd_cli11 multiply 4 6     # 4 * 6 = 24
./build/src/cmd_cli11 divide 20 4      # 20 / 4 = 5

# 設定ファイル指定
./build/src/cmd_cli11 --config config/example.toml add 10 20
```

#### morrisfranken argparse版

```bash
# Morris argparse版の使用例（キーワード引数）
./build/src/cmd_argparse_morris add --a 10 --b 20        # 10 + 20 = 30
./build/src/cmd_argparse_morris subtract --a 15 --b 5    # 15 - 5 = 10
./build/src/cmd_argparse_morris multiply --a 4 --b 6     # 4 * 6 = 24
./build/src/cmd_argparse_morris divide --a 20 --b 4      # 20 / 4 = 5

# 設定ファイル指定
./build/src/cmd_argparse_morris --config config/example.toml add --a 10 --b 20
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

## ライブラリ比較

### CLI11の特徴

- **実装方式**: callback方式（add, subtract）とgot_subcommand方式（multiply, divide）
- **メリット**:
  - 豊富な機能
  - 2つの実装パターンを学習可能
  - 直感的なAPI
- **使用方法**: 位置引数ベース

### morrisfranken argparseの特徴

- **実装方式**: 構造体継承を使用したオブジェクト指向アプローチ
- **メリット**:
  - 軽量で高速
  - コンパイル時間が短い
  - シンプルな設計
- **使用方法**: キーワード引数ベース

## 使用しているライブラリ

| 技術 | 用途 | 理由 |
|------|------|------|
| **CLI11** | コマンドライン引数解析 | 直感的なAPI、サブコマンド対応 |
| **morrisfranken argparse** | コマンドライン引数解析 | 軽量、構造体継承による型安全性 |
| **tomlplusplus** | 設定ファイル解析 | 高性能なモダンTOMLライブラリ |
| **fmt** | 文字列フォーマット | 高性能で安全な文字列操作 |
| **doctest** | テストフレームワーク | 軽量でヘッダーオンリー |

## 開発ツール

### コード品質管理

```bash
# フォーマット
cmake --build build --target format

# リント
cmake --build build --target lint

# 静的解析
cmake --build build --target run-cppcheck

# 全品質チェック
cmake --build build --target fullcheck

# テスト実行
cmake --build build --target run-tests

# 個別テスト実行
./build/tests/cpp/test_sut_example

# 利用可能なターゲット表示
cmake --build build --target show-help

# ライセンス情報の収集
cmake --build build --target collect-licenses
```

### 個別ビルド

```bash
# CLI11版のみビルド
cmake --build build --target cmd_cli11

# Morris argparse版のみビルド
cmake --build build --target cmd_argparse_morris
```

### コード品質管理ツール

- **clang-format**: コードフォーマット
- **clang-tidy**: 静的解析とリント
- **cppcheck**: 追加の静的解析
- **doctest**: ユニットテスト

## ビルド時間比較

各ライブラリのビルド時間比較（makeビルドシステム使用時）：

| ライブラリ | ビルド時間 | 特徴 |
|------------|-----------|------|
| **morrisfranken argparse** | 1.429秒 | 最高速 |
| **CLI11** | 3.685秒 | 多機能だがビルド時間が長い |

## 拡張方法

### 新しいサブコマンドの追加

#### CLI11版
1. `src/command/subcommand_cli11.cpp` に新しい関数を追加
2. `SetCallbackSubcommands` または `SetGotSubcommands` に登録

#### Morris argparse版
1. `include/command/subcommand_argparse_morris.hpp` に新しい構造体を定義
2. `src/command/subcommand_argparse_morris.cpp` に `run()` メソッドを実装
3. メインの引数構造体に新しいサブコマンドを追加

### 設定項目の追加

1. `include/config/config_loader.hpp` で設定構造体を定義
2. `config.toml` に新しい設定項目を追加
3. 設定読み込み処理を更新

## 設定ファイルの使用

### TOML設定ファイル

プロジェクトには設定ファイルサンプル `config/example.toml` が含まれています：

```toml
[app]
name = "template_cli_cpp"
version = "0.1.0"
description = "C++ CLI Template with multiple library examples"

[logging]
level = "info"
enable_file_output = false
log_file = "app.log"

[features]
enable_config_validation = true
verbose_output = false
```

### VSCode統合

プロジェクトには`.vscode/launch.json`と`.vscode/tasks.json`が含まれており、VSCode上でデバッグや各種ビルドタスクを実行できます。

#### 利用可能なタスク
- Conan依存関係インストール
- CMake設定
- 各ライブラリの個別ビルド
- デモ実行
- コード品質チェック

#### デバッグ設定
- CLI11版のデバッグ設定
- Morris argparse版のデバッグ設定
- テストのデバッグ設定

## ライセンス管理

サードパーティライブラリのライセンス情報は以下のコマンドで集約できます：

```bash
# ライセンス情報の収集
cmake --build build --target collect-licenses

# 収集されたライセンスファイルの確認
ls build/third_party_licenses/
```

ライセンス情報は`build/third_party_licenses/`ディレクトリに集約されます。Conanで管理されているライブラリ（CLI11、tomlplusplus、fmt、doctest）とFetchContentで取得されているライブラリ（morrisfranken argparse）の両方のライセンス情報が含まれます。