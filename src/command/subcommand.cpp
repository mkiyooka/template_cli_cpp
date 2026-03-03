#include <memory>

#include <CLI/CLI.hpp>
#include <fmt/base.h>

#include "command/subcommand.hpp"

// Callback方式用の構造体
struct CallbackOptions {
    int a;
    int b;
};

// 実行関数群
void ExecuteAdd(const CallbackOptions &options) { fmt::print("{} + {} = {}\n", options.a, options.b, options.a + options.b); }

void ExecuteSubtract(const CallbackOptions &options) { fmt::print("{} - {} = {}\n", options.a, options.b, options.a - options.b); }

void ExecuteMultiply(const SubcommandOptions &options) { fmt::print("{} * {} = {}\n", options.a, options.b, options.a * options.b); }

void ExecuteDivide(const SubcommandOptions &options) {
    if (options.b == 0) {
        fmt::print("Error: Division by zero\n");
        return;
    }
    fmt::print("{} / {} = {}\n", options.a, options.b, static_cast<double>(options.a) / options.b);
}

// callback方式のサブコマンド設定
void SetCallbackSubcommands(CLI::App &app) {
    // add subcommand (callback方式)
    {
        auto add_options = std::make_shared<CallbackOptions>();
        auto *subcommand = app.add_subcommand("add", "Addition operation");
        subcommand->add_option("a", add_options->a, "First operand")->required();
        subcommand->add_option("b", add_options->b, "Second operand")->required();
        subcommand->callback([add_options]() { ExecuteAdd(*add_options); });
    }
    // subtract subcommand (callback方式)
    {
        auto subtract_options = std::make_shared<CallbackOptions>();
        auto *subcommand = app.add_subcommand("subtract", "Subtraction operation");
        subcommand->add_option("a", subtract_options->a, "First operand")->required();
        subcommand->add_option("b", subtract_options->b, "Second operand")->required();
        subcommand->callback([subtract_options]() { ExecuteSubtract(*subtract_options); });
    }
}

// got_subcommand方式のサブコマンド設定
void SetGotSubcommands(CLI::App &app, SubcommandOptions &multiply_options, SubcommandOptions &divide_options) {
    // multiply subcommand (got_subcommand方式)
    {
        auto *subcommand = app.add_subcommand("multiply", "Multiplication operation");
        subcommand->add_option("a", multiply_options.a, "First operand")->required();
        subcommand->add_option("b", multiply_options.b, "Second operand")->required();
    }
    // divide subcommand (got_subcommand方式)
    {
        auto *subcommand = app.add_subcommand("divide", "Division operation");
        subcommand->add_option("a", divide_options.a, "First operand")->required();
        subcommand->add_option("b", divide_options.b, "Second operand")->required();
    }
}

// got_subcommand方式の実行処理
void ExecuteGotSubcommands(CLI::App &app, const SubcommandOptions &multiply_options, const SubcommandOptions &divide_options) {
    if (app.got_subcommand("multiply")) {
        ExecuteMultiply(multiply_options);
    } else if (app.got_subcommand("divide")) {
        ExecuteDivide(divide_options);
    }
}
