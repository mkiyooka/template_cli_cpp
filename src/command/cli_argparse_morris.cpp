#include <iostream>
#include <stdexcept>

#include <fmt/base.h>

#include "command/cli_argparse_morris.hpp"
#include "command/subcommand_argparse_morris.hpp"
#include "config/config_loader.hpp"

int CliArgparseMorris(int argc, char *argv[]) {
    try {
        // argparseを使用して引数を解析
        auto args = argparse::parse<ArgparseMorrisArgs>(argc, argv);

        // サブコマンドを実行
        int result = args.run_subcommands();

        // 設定ファイルの読み込みと表示
        Config conf;
        LoadConfig(args.config_file, conf);
        ShowConfig(conf);

        return result;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n" << std::flush;
        return 1;
    }
}
