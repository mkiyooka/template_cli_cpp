#include <CLI/CLI.hpp>
#include <fmt/base.h>

#include "command/subcommand.hpp"

// サブコマンドマッピング（config_file_loader.cpp から参照される）
// サブコマンドを追加・変更する場合はここと下の Set*Subcommands を修正する
const SubcommandMapping kSubcommandMappings[] = {
    {     "add",      &Config::add},
    {"subtract", &Config::subtract},
    {"multiply", &Config::multiply},
    {  "divide",   &Config::divide},
};
const std::size_t kSubcommandMappingCount = std::size(kSubcommandMappings);

// 実行関数群
void ExecuteAdd(const SubcommandConfig &config) {
    fmt::print("{} + {} = {}\n", config.a, config.b, config.a + config.b);
}

void ExecuteSubtract(const SubcommandConfig &config) {
    fmt::print("{} - {} = {}\n", config.a, config.b, config.a - config.b);
}

void ExecuteMultiply(const SubcommandConfig &config) {
    fmt::print("{} * {} = {}\n", config.a, config.b, config.a * config.b);
}

void ExecuteDivide(const SubcommandConfig &config) {
    if (config.b == 0) {
        fmt::print("Error: Division by zero\n");
        return;
    }
    fmt::print("{} / {} = {}\n", config.a, config.b, static_cast<double>(config.a) / config.b);
}

// callback方式のサブコマンド設定
void SetCallbackSubcommands(CLI::App &app, Config &config) {
    // add subcommand (callback方式)
    {
        auto *subcommand = app.add_subcommand("add", "Addition operation");
        subcommand->add_option("a", config.add.a, "First operand")->required();
        subcommand->add_option("b", config.add.b, "Second operand")->required();
        subcommand->callback([&config]() { ExecuteAdd(config.add); });
    }
    // subtract subcommand (callback方式)
    {
        auto *subcommand = app.add_subcommand("subtract", "Subtraction operation");
        subcommand->add_option("a", config.subtract.a, "First operand")->required();
        subcommand->add_option("b", config.subtract.b, "Second operand")->required();
        subcommand->callback([&config]() { ExecuteSubtract(config.subtract); });
    }
}

// got_subcommand方式のサブコマンド設定
void SetGotSubcommands(CLI::App &app, Config &config) {
    // multiply subcommand (got_subcommand方式)
    {
        auto *subcommand = app.add_subcommand("multiply", "Multiplication operation");
        subcommand->add_option("a", config.multiply.a, "First operand")->required();
        subcommand->add_option("b", config.multiply.b, "Second operand")->required();
    }
    // divide subcommand (got_subcommand方式)
    {
        auto *subcommand = app.add_subcommand("divide", "Division operation");
        subcommand->add_option("a", config.divide.a, "First operand")->required();
        subcommand->add_option("b", config.divide.b, "Second operand")->required();
    }
}

// got_subcommand方式の実行処理
void ExecuteGotSubcommands(CLI::App &app, const Config &config) {
    if (app.got_subcommand("multiply")) {
        ExecuteMultiply(config.multiply);
    } else if (app.got_subcommand("divide")) {
        ExecuteDivide(config.divide);
    }
}
