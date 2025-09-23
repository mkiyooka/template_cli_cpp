#include <iostream>
#include <fmt/base.h>

#include "command/subcommand_argparse_morris.hpp"

// AddArgs実装
int AddArgs::run() {
    fmt::print("{} + {} = {}\n", a, b, a + b);
    return 0;
}

// SubtractArgs実装
int SubtractArgs::run() {
    fmt::print("{} - {} = {}\n", a, b, a - b);
    return 0;
}

// MultiplyArgs実装
int MultiplyArgs::run() {
    fmt::print("{} * {} = {}\n", a, b, a * b);
    return 0;
}

// DivideArgs実装
int DivideArgs::run() {
    if (b == 0) {
        fmt::print("Error: Division by zero\n");
        return 1;
    }
    fmt::print("{} / {} = {}\n", a, b, static_cast<double>(a) / b);
    return 0;
}