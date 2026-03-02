# 出力システム利用ガイド

設計・実装の詳細は [output-system.md](output-system.md) を参照。

---

## 基本的な使い方

### 1. モジュールキーを定義する

出力先をモジュール単位で切り替えるために `enum class` を定義する。

```cpp
enum class Module { Solver, Postproc };
```

### 2. Logger と RecorderManager を初期化する

```cpp
#include "template_cli_cpp/logging/logger_factory.hpp"
#include "template_cli_cpp/output/app_output.hpp"
#include "template_cli_cpp/recording/recorder_factory.hpp"
#include "template_cli_cpp/recording/recorder_manager.hpp"

// 診断ログ: コンソール出力、Info レベル以上
auto logger = LoggerFactory::MakeConsole("app", LogLevel::Info);

// 解析データ: モジュールごとにファイル出力
RecorderManager<Module> manager;
manager.RegisterRecorder(Module::Solver,   RecorderFactory::MakeFile("solver",   "solver.csv"));
manager.RegisterRecorder(Module::Postproc, RecorderFactory::MakeFile("postproc", "postproc.csv"));
```

### 3. AppOutput にまとめて注入する

```cpp
AppOutput<Module> out(*logger, manager);
run(out);
```

---

## Logger の使い方

### ログレベルと出力制御

```cpp
Logger& log = out.logger();

log.Log(LogLevel::Trace,    "詳細トレース");
log.Log(LogLevel::Debug,    "デバッグ情報");
log.Log(LogLevel::Info,     "進行状況");
log.Log(LogLevel::Warn,     "警告");
log.Log(LogLevel::Error,    "エラー");
log.Log(LogLevel::Critical, "致命的エラー");
```

コスト高な文字列生成はレベル確認後に行う。

```cpp
if (log.ShouldLog(LogLevel::Debug)) {
    log.Log(LogLevel::Debug, expensive_to_string(state));
}
```

### ファクトリ一覧

| メソッド                              | 出力先                 |
| ------------------------------------- | ---------------------- |
| `LoggerFactory::MakeConsole(name)`    | 標準出力（カラー付き） |
| `LoggerFactory::MakeFile(name, path)` | ファイル（同期）       |
| `LoggerFactory::MakeNull()`           | 何もしない             |

`MakeConsole` / `MakeFile` は第3引数でログレベルを指定できる（省略時は `Info`）。

---

## DataRecorder の使い方

### Enable / Disable と Write

```cpp
DataRecorder& rec = out.recorders()[Module::Solver];

rec.Enable();                              // 記録開始
rec.Write("{},{:.6f}", step, value);       // fmt 形式で書き込み
rec.Write("{},{},{}", t, x, y);
rec.Disable();                             // 記録停止（以降の Write は無視される）
rec.Flush();                               // バッファをフラッシュ
```

`Write()` はフォーマット文字列をコンパイル時にチェックする（`fmt::format_string`）。
`IsEnabled()` が `false` のときはフォーマット処理自体をスキップするため、無効時のコストはゼロ。

### 全レコーダーを一括フラッシュする

```cpp
out.recorders().FlushAll();
```

### ファクトリ一覧

| メソッド                                | 出力先           | 初期状態 |
| --------------------------------------- | ---------------- | -------- |
| `RecorderFactory::MakeFile(name, path)` | ファイル（同期） | disabled |
| `RecorderFactory::MakeNull()`           | 何もしない       | disabled |

`MakeFile` で生成したレコーダーは初期状態が `disabled`。
記録を開始するには明示的に `Enable()` を呼ぶ。

---

## テストでの使い方

テストでは `NullLogger` と `TestLogger` を使い、外部 I/O なしに動作を検証できる。

```cpp
#include "template_cli_cpp/logging/null_logger.hpp"
#include "template_cli_cpp/logging/test_logger.hpp"
#include "template_cli_cpp/recording/null_recorder.hpp"

// ログ検証が不要な場合
NullLogger null_log;

// ログ内容を検証したい場合
TestLogger test_log;
test_log.set_level(LogLevel::Debug);

RecorderManager<Module> manager;
manager.RegisterRecorder(Module::Solver, std::make_shared<NullRecorder>());

AppOutput<Module> out(test_log, manager);
run(out);

assert(test_log.entries().size() == 3);
assert(test_log.entries()[0] == "initialize start");

test_log.clear(); // エントリをリセット
```

---

## よくあるパターン

### モジュールごとに出力を切り替える

```cpp
void run(AppOutput<Module>& out) {
    out.recorders()[Module::Solver].Enable();
    out.recorders()[Module::Postproc].Disable(); // この回は不要

    for (int step = 0; step < steps; ++step) {
        double x = compute(step);
        out.recorders()[Module::Solver].Write("{},{:.9f}", step, x);
        out.logger().Log(LogLevel::Debug, "step done");
    }

    out.recorders().FlushAll();
}
```

### 出力を完全に無効化する（本番ランの高速化）

```cpp
auto logger = LoggerFactory::MakeNull();

RecorderManager<Module> manager;
manager.RegisterRecorder(Module::Solver,   RecorderFactory::MakeNull());
manager.RegisterRecorder(Module::Postproc, RecorderFactory::MakeNull());

AppOutput<Module> out(*logger, manager);
run(out); // I/O コストゼロ
```

### ログレベルを動的に変更する

```cpp
out.logger().set_level(LogLevel::Warn); // Debug・Info を抑制
```
