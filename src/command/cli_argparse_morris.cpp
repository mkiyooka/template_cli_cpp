#include <exception>
#include <iostream>

#include <fmt/base.h>

#include "command/cli_argparse_morris.hpp"
#include "command/subcommand_argparse_morris.hpp"
#include "config/config_manager.hpp"

int CliArgparseMorris(int argc, char *argv[]) {
    try {
        // argparseを使用して引数を解析
        auto args = argparse::parse<ArgparseMorrisArgs>(argc, argv);

        // サブコマンドを実行
        const int result = args.run_subcommands();

        // 設定の解決と表示
        config::ConfigManager mgr;
        const Config conf = mgr.Resolve(args.config_file);
        config::ShowConfig(conf);

        return result;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n" << std::flush;
        return 1;
    }
}
