#include <memory>

#include <CLI/CLI.hpp>
#include <fmt/base.h>

#include "command/subcommand_add.hpp"

struct SubcommandOptions {
    int a;
    int b;
};

void ExecuteAdd(SubcommandOptions &opt) { fmt::print("{} + {} = {}\n", opt.a, opt.b, opt.a + opt.b); }

void SetSubcommandAdd(CLI::App &app) {
    auto opt = std::make_shared<SubcommandOptions>();
    auto *sub = app.add_subcommand("add", "addition method");

    sub->add_option("a", opt->a, "First operand")->required();
    sub->add_option("b", opt->b, "Second operand")->required();

    sub->callback([opt]() { ExecuteAdd(*opt); });
}
