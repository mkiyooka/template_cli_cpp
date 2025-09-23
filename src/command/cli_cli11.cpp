#include <cstdlib>

#include <CLI/CLI.hpp>
#include <fmt/base.h>

#include "command/subcommand_cli11.hpp"
#include "config/config_loader.hpp"

int CliCLI11(int argc, char *argv[]) {
    CLI::App app{"CLI11 demonstration with different subcommand styles"};
    argv = app.ensure_utf8(argv);
    std::string config_file;
    app.add_option("-c,--config", config_file, "Configuration file");

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

    // 設定ファイルの読み込みと表示
    Config conf;
    LoadConfig(config_file, conf);
    ShowConfig(conf);

    return 0;
}
