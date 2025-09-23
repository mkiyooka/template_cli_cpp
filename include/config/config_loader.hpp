#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct PluginConfig {
    std::string file;
    std::uint64_t number;
};
struct Config {
    std::string title = "title";
    std::uint64_t value = 10;
    std::vector<struct PluginConfig> plugins;
};

void LoadConfig(const std::string &file_path, Config &conf);
void ShowConfig(const Config &conf);
