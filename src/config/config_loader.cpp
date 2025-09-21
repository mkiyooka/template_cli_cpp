#include <string>

#include <toml++/toml.hpp>

#include "config/config_loader.hpp"

void LoadConfig(const std::string &file_path, Config &conf) {
    if (file_path.empty()) {
        return;
    }
    auto toml_config = toml::parse_file(file_path);
    conf.title = toml_config["title"].value_or("");
    conf.value = toml_config["value"].value_or(0);
}
