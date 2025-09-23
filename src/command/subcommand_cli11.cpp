#include <memory>

#include <CLI/CLI.hpp>
#include <fmt/base.h>

#include "command/subcommand_cli11.hpp"

// Callback方式用の構造体
struct CallbackOptions {
    int a;
    int b;
};

// 実行関数群
void ExecuteAdd(const CallbackOptions &opt) { fmt::print("{} + {} = {}\n", opt.a, opt.b, opt.a + opt.b); }

void ExecuteSubtract(const CallbackOptions &opt) { fmt::print("{} - {} = {}\n", opt.a, opt.b, opt.a - opt.b); }

void ExecuteMultiply(const GotSubcommandOptions &opt) { fmt::print("{} * {} = {}\n", opt.a, opt.b, opt.a * opt.b); }

void ExecuteDivide(const GotSubcommandOptions &opt) {
    if (opt.b == 0) {
        fmt::print("Error: Division by zero\n");
        return;
    }
    fmt::print("{} / {} = {}\n", opt.a, opt.b, static_cast<double>(opt.a) / opt.b);
}

// callback方式のサブコマンド設定
void SetCallbackSubcommands(CLI::App &app) {
    // add subcommand (callback方式)
    {
        auto add_opt = std::make_shared<CallbackOptions>();
        auto *subcommand = app.add_subcommand("add", "Addition operation");
        subcommand->add_option("a", add_opt->a, "First operand")->required();
        subcommand->add_option("b", add_opt->b, "Second operand")->required();
        subcommand->callback([add_opt]() { ExecuteAdd(*add_opt); });
    }
    // subtract subcommand (callback方式)
    {
        auto sub_opt = std::make_shared<CallbackOptions>();
        auto *subcommand = app.add_subcommand("subtract", "Subtraction operation");
        subcommand->add_option("a", sub_opt->a, "First operand")->required();
        subcommand->add_option("b", sub_opt->b, "Second operand")->required();
        subcommand->callback([sub_opt]() { ExecuteSubtract(*sub_opt); });
    }
}

// got_subcommand方式のサブコマンド設定
void SetGotSubcommands(CLI::App &app, GotSubcommandOptions &mul_opt, GotSubcommandOptions &div_opt) {
    // multiply subcommand (got_subcommand方式)
    {
        auto *subcommand = app.add_subcommand("multiply", "Multiplication operation");
        subcommand->add_option("a", mul_opt.a, "First operand")->required();
        subcommand->add_option("b", mul_opt.b, "Second operand")->required();
    }
    // divide subcommand (got_subcommand方式)
    {
        auto *subcommand = app.add_subcommand("divide", "Division operation");
        subcommand->add_option("a", div_opt.a, "First operand")->required();
        subcommand->add_option("b", div_opt.b, "Second operand")->required();
    }
}

// got_subcommand方式の実行処理
void ExecuteGotSubcommands(CLI::App &app, const GotSubcommandOptions &mul_opt, const GotSubcommandOptions &div_opt) {
    if (app.got_subcommand("multiply")) {
        ExecuteMultiply(mul_opt);
    } else if (app.got_subcommand("divide")) {
        ExecuteDivide(div_opt);
    }
}
