#include <cstdlib>

#include <CLI/CLI.hpp>
#include <fmt/base.h>

#include "command/subcommand_add.hpp"
#include "command/subcommand_mul.hpp"
#include "config/config_loader.hpp"

int Cli(int argc, char *argv[]) {
    CLI::App app{"cmd description"};
    argv = app.ensure_utf8(argv);
    std::string config_file;
    app.add_option("-c,--config", config_file, "default filename");

    SetSubcommandAdd(app);
    SetSubcommandMul(app);

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &e) {
        std::exit(app.exit(e)); // app.exit(e) prints help
    }
    Config conf;
    LoadConfig(config_file, conf);
    fmt::print("title: {}, value: {}\n", conf.title, conf.value);
    return 0;
}
