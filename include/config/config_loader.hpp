#pragma once

#include <cstdint>
#include <string>

struct Config {
    std::string title = "title";
    std::uint64_t value = 10;
};

void LoadConfig(const std::string &file_path, Config &conf);
