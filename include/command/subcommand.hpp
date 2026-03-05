#pragma once

#include <CLI/CLI.hpp>

#include "config/config_loader.hpp"

// callback方式のサブコマンド設定
void SetCallbackSubcommands(CLI::App &app, Config &config);

// got_subcommand方式のサブコマンド設定
void SetGotSubcommands(CLI::App &app, Config &config);

// got_subcommand方式の実行処理
void ExecuteGotSubcommands(CLI::App &app, const Config &config);
