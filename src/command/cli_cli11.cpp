#include <cstdlib>
#include <memory>

#include <CLI/CLI.hpp>
#include <fmt/base.h>
#include <fmt/format.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "command/subcommand_cli11.hpp"
#include "config/config_manager.hpp"
#include "sut_example/system_under_test.hpp"
#include "template_cli_cpp/logging/logger_factory.hpp"
#include "template_cli_cpp/output/app_output.hpp"
#include "template_cli_cpp/recording/recorder_manager.hpp"
#include "template_cli_cpp/recording/spdlog_recorder.hpp"
#include "utility/yyjson_wrapper.hpp"

namespace {

enum class OutputModule : std::uint8_t { kResults };

// DataRecorder をコンソール（stdout）へ出力するレコーダーを生成する
std::unique_ptr<DataRecorder> MakeConsoleRecorder(const std::string &name) {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto inner = std::make_shared<spdlog::logger>(name, sink);
    return std::make_unique<SpdlogRecorder>(inner);
}

// Logger と DataRecorder を使った出力サンプル
//
// - 入力変数は Logger で診断ログとして出力
// - 計算結果は JsonBuilder で JSON を構築し DataRecorder で出力
void RunOutputSample(AppOutput<OutputModule> &out) {
    Logger &log = out.logger();
    DataRecorder &rec = out.recorders()[OutputModule::kResults];

    // --- 入力パラメータ ---
    const double input = 3.5;
    const int start = 1;
    const int count = 5;
    const int divisor = 7;
    const int target_value = 15;

    // 入力変数をログに出力
    log.Log(LogLevel::Info, "=== output sample start ===");
    log.Log(
        LogLevel::Info,
        fmt::format(
            "input={}, start={}, count={}, divisor={}, target_value={}", input, start, count, divisor, target_value
        )
    );

    // --- 計算 ---
    const double doubled = sut_example::DoubleValue(input);
    const auto sequence = sut_example::CreateSequence(start, count);
    const sut_example::ModuloCalculator calc(divisor);
    const int remainder = calc.GetRemainder(target_value);

    log.Log(LogLevel::Debug, fmt::format("DoubleValue({}) = {}", input, doubled));
    log.Log(LogLevel::Debug, fmt::format("CreateSequence({}, {}) size={}", start, count, sequence.size()));
    log.Log(LogLevel::Debug, fmt::format("GetRemainder({}) = {}", target_value, remainder));

    // --- JSON 出力 ---
    rec.Enable();

    json::JsonBuilder builder;

    // inputs サブオブジェクト
    auto inputs_obj = builder.AddNested("inputs");
    builder.AddToNested(inputs_obj, "input", input);
    builder.AddToNested(inputs_obj, "start", start);
    builder.AddToNested(inputs_obj, "count", count);
    builder.AddToNested(inputs_obj, "divisor", divisor);
    builder.AddToNested(inputs_obj, "target_value", target_value);

    // results サブオブジェクト（sequence は vector<int> として Add() を使用）
    auto results_obj = builder.AddNested("results");
    builder.AddToNested(results_obj, "doubled", doubled);
    builder.AddToNested(results_obj, "remainder", remainder);
    // sequence は vector<int> なのでトップレベルの Add() で追加
    builder.Add("sequence", sequence);

    rec.Write("{}", builder.Serialize(/* pretty = */ false));

    rec.Flush();
    rec.Disable();

    log.Log(LogLevel::Info, "=== output sample end ===");
}

} // namespace

int CliCLI11(int argc, char *argv[]) {
    CLI::App app{"CLI11 demonstration with different subcommand styles"};
    argv = app.ensure_utf8(argv);

    std::string config_file;
    app.add_option("-c,--config", config_file, "Configuration file");

    config::ConfigManager mgr;
    mgr.RegisterOptions(app);

    // callback方式のサブコマンド (add, subtract)
    SetCallbackSubcommands(app);

    // got_subcommand方式のサブコマンド (multiply, divide)
    GotSubcommandOptions mul_opt;
    GotSubcommandOptions div_opt;
    SetGotSubcommands(app, mul_opt, div_opt);

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &e) {
        std::exit(app.exit(e)); // app.exit(e) prints help
    }

    // got_subcommand方式のサブコマンド実行
    ExecuteGotSubcommands(app, mul_opt, div_opt);

    // 設定の解決と表示
    const Config conf = mgr.Resolve(config_file);
    config::ShowConfig(conf);

    // 出力サンプル: Logger でログ出力、DataRecorder で JSON 結果を出力
    auto logger = LoggerFactory::MakeConsole("app", LogLevel::Debug);
    RecorderManager<OutputModule> manager;
    manager.RegisterRecorder(OutputModule::kResults, MakeConsoleRecorder("results"));
    AppOutput<OutputModule> out(*logger, manager);
    RunOutputSample(out);

    return 0;
}
