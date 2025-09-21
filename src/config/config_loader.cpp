#include <string>

#include <toml++/toml.hpp>

#include "config/config_loader.hpp"

void LoadConfig(const std::string &file_path, Config &conf) {
    if (file_path.empty()) {
        return;
    }
    auto toml_config = toml::parse_file(file_path);
    conf.title = toml_config["title"].value_or("");
    conf.value = toml_config["settings"]["value"].value_or(0);

    // lambda関数方式
    if (auto *arr = toml_config["pluginA"].as_array()) {
        arr->for_each([&conf](auto &&el) {
            if (auto *table = el.as_table()) {
                PluginConfig plugin;
                plugin.file = (*table)["file"].value_or("");
                plugin.number = (*table)["number"].value_or(0);
                conf.plugins_a.push_back(plugin);
            }
        });
    }
    // forループ方式
    if (auto *arr = toml_config["pluginB"].as_array()) {
        for (auto &&el : *arr) {
            if (auto *table = el.as_table()) {
                PluginConfig plugin;
                plugin.file = (*table)["file"].value_or("");
                plugin.number = (*table)["number"].value_or(0);
                conf.plugins_b.push_back(plugin);
            }
        }
    }
}
