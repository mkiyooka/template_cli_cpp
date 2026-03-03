#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct PluginConfig {
    std::string file;
    std::uint64_t number = 0;
};

struct Config {
    std::string title = "title";
    std::uint64_t value = 10;
    std::vector<PluginConfig> plugins;
};
