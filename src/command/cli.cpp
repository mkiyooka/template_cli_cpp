#include <cstdlib>
#include <memory>

#include <CLI/CLI.hpp>
#include <fmt/base.h>
#include <fmt/format.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "command/subcommand.hpp"
#include "config/config_manager.hpp"
#include "config/config_schema.hpp"
#include "sut_example/system_under_test.hpp"
#include "template_cli_cpp/logging/logger_factory.hpp"
#include "template_cli_cpp/output/app_output.hpp"
#include "template_cli_cpp/recording/recorder_manager.hpp"
#include "template_cli_cpp/recording/spdlog_recorder.hpp"
#include "utility/yyjson_wrapper.hpp"

namespace {

enum class OutputModule : std::uint8_t { kResults };

// 設定内容をターミナルに表示する（デバッグ・確認用）
void ShowConfig(const Config &conf) {
    std::apply(
        [&](auto &&...field) { ([&] { fmt::print("{}: {}\n", field.config_key, conf.*field.member); }(), ...); },
        config::kConfigSchema
    );
    for (const auto &p : conf.plugins) {
        fmt::print("plugin: file={}, number={}\n", p.file, p.number);
    }
    for (std::size_t i = 0; i < kSubcommandMappingCount; ++i) {
        const auto &m = kSubcommandMappings[i];
        fmt::print("subcommands.{}: a={}, b={}\n", m.key, (conf.*m.member).a, (conf.*m.member).b);
    }
}

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
    Logger &logger = out.GetLogger();
    DataRecorder &recorder = out.GetRecorders()[OutputModule::kResults];

    // --- 入力パラメータ ---
    const double input = 3.5;
    const int start = 1;
    const int count = 5;
    const int divisor = 7;
    const int target_value = 15;

    // 入力変数をログに出力
    logger.Log(LogLevel::Info, "=== output sample start ===");
    logger.Log(
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

    logger.Log(LogLevel::Debug, fmt::format("DoubleValue({}) = {}", input, doubled));
    logger.Log(LogLevel::Debug, fmt::format("CreateSequence({}, {}) size={}", start, count, sequence.size()));
    logger.Log(LogLevel::Debug, fmt::format("GetRemainder({}) = {}", target_value, remainder));

    // --- JSON 出力 ---
    recorder.Enable();

    json::JsonBuilder builder;

    // inputs サブオブジェクト
    auto inputs_object = builder.AddNested("inputs");
    builder.AddToNested(inputs_object, "input", input);
    builder.AddToNested(inputs_object, "start", start);
    builder.AddToNested(inputs_object, "count", count);
    builder.AddToNested(inputs_object, "divisor", divisor);
    builder.AddToNested(inputs_object, "target_value", target_value);

    // results サブオブジェクト（sequence は vector<int> として Add() を使用）
    auto results_object = builder.AddNested("results");
    builder.AddToNested(results_object, "doubled", doubled);
    builder.AddToNested(results_object, "remainder", remainder);
    // sequence は vector<int> なのでトップレベルの Add() で追加
    builder.Add("sequence", sequence);

    recorder.Write("{}", builder.Serialize(/* pretty = */ false));

    recorder.Flush();
    recorder.Disable();

    logger.Log(LogLevel::Info, "=== output sample end ===");
}

} // namespace

int RunCli(int argc, char *argv[]) {
    CLI::App app{"Command line parser demonstration with different subcommand styles"};
    argv = app.ensure_utf8(argv);

    std::string config_file;
    app.add_option("-c,--config", config_file, "Configuration file");

    config::ConfigManager config_manager;
    config_manager.RegisterOptions(app);

    Config config; // CLI サブコマンドオプションのバインド先

    // callback方式のサブコマンド (add, subtract)
    SetCallbackSubcommands(app, config);

    // got_subcommand方式のサブコマンド (multiply, divide)
    SetGotSubcommands(app, config);

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &e) {
        std::exit(app.exit(e)); // app.exit(e) prints help
    }

    // got_subcommand方式のサブコマンド実行
    ExecuteGotSubcommands(app, config);

    // スキーマフィールドを解決（CLI引数 > 設定ファイル > デフォルト値）
    const Config resolved = config_manager.Resolve(config_file);
    config.title = resolved.title;
    config.value = resolved.value;

    // スキーマ外フィールドはファイルの生値から取得
    const Config &file_vals = config_manager.GetFileValues();
    config.plugins = file_vals.plugins;

    // サブコマンドが CLI から指定されていれば config の値を優先、未指定ならファイル値を使う
    for (std::size_t i = 0; i < kSubcommandMappingCount; ++i) {
        const auto &m = kSubcommandMappings[i];
        if (!app.got_subcommand(m.key)) {
            config.*m.member = file_vals.*m.member;
        }
    }

    ShowConfig(config);

    // 出力サンプル: Logger でログ出力、DataRecorder で JSON 結果を出力
    auto logger = LoggerFactory::MakeConsole("app", LogLevel::Debug);
    RecorderManager<OutputModule> recorder_manager;
    recorder_manager.RegisterRecorder(OutputModule::kResults, MakeConsoleRecorder("results"));
    AppOutput<OutputModule> out(*logger, recorder_manager);
    RunOutputSample(out);

    return 0;
}
