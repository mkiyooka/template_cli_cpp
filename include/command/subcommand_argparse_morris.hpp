#pragma once

#include <argparse/argparse.hpp>
#include <string>

// NOLINTBEGIN
// サブコマンド実装
struct AddArgs : public argparse::Args {
    int &a = arg("First operand");
    int &b = arg("Second operand");

    int run() override;
};

struct SubtractArgs : public argparse::Args {
    int &a = kwarg("a", "First operand");
    int &b = kwarg("b", "Second operand");

    int run() override;
};

struct MultiplyArgs : public argparse::Args {
    int &a = arg("First operand");
    int &b = arg("Second operand");

    int run() override;
};

struct DivideArgs : public argparse::Args {
    int &a = kwarg("a", "First operand");
    int &b = kwarg("b", "Second operand");

    int run() override;
};

// メインの引数構造体
struct ArgparseMorrisArgs : public argparse::Args {
    std::string &config_file = kwarg("c,config", "Configuration file").set_default("config.toml");
    AddArgs &add = subcommand("add");
    SubtractArgs &subtract = subcommand("subtract");
    MultiplyArgs &multiply = subcommand("multiply");
    DivideArgs &divide = subcommand("divide");
};
// NOLINTEND
