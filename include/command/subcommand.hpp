#pragma once

#include <CLI/CLI.hpp>

// got_subcommand方式用の構造体
struct SubcommandOptions {
    int a = 0;
    int b = 0;
};

// callback方式のサブコマンド設定
void SetCallbackSubcommands(CLI::App &app);

// got_subcommand方式のサブコマンド設定
void SetGotSubcommands(CLI::App &app, SubcommandOptions &multiply_options, SubcommandOptions &divide_options);

// got_subcommand方式の実行処理
void ExecuteGotSubcommands(CLI::App &app, const SubcommandOptions &multiply_options, const SubcommandOptions &divide_options);
