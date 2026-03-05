#include "config/config_file_loader.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <fkYAML/node.hpp>
#include <nlohmann/json.hpp>
#include <toml++/toml.hpp>

#include "config/config_schema.hpp"

namespace config {

namespace {

// ──────────────────────────────────────────────
// TOML ユーティリティ
// ──────────────────────────────────────────────

template <typename T>
std::optional<T> ResolveTomlKey(const toml::table &tbl, std::string_view dotted_key) {
    const toml::table *current = &tbl;
    std::string_view remaining = dotted_key;

    while (true) {
        const auto dot_pos = remaining.find('.');
        if (dot_pos == std::string_view::npos) {
            return (*current)[remaining].template value<T>();
        }
        const auto head = remaining.substr(0, dot_pos);
        remaining = remaining.substr(dot_pos + 1);
        current = (*current)[head].as_table();
        if (current == nullptr) {
            return std::nullopt;
        }
    }
}

void LoadFromToml(const std::string &file_path, Config &conf) {
    const auto tbl = toml::parse_file(file_path);

    // スキーマフィールドを自動マッピング
    std::apply(
        [&](auto &&...field) {
            (
                [&] {
                    using FieldType = std::remove_reference_t<decltype(conf.*field.member)>;
                    auto val = ResolveTomlKey<FieldType>(tbl, field.config_key);
                    if (val.has_value()) {
                        conf.*field.member = *val;
                    }
                }(),
                ...
            );
        },
        kConfigSchema
    );

    // plugins 配列 (スキーマ対象外の複合型)
    if (const auto *arr = tbl["plugin"].as_array()) {
        conf.plugins.clear();
        for (const auto &el : *arr) {
            if (const auto *table = el.as_table()) {
                PluginConfig plugin;
                plugin.file = (*table)["file"].value_or(std::string{});
                plugin.number = (*table)["number"].value_or(std::uint64_t{0});
                conf.plugins.push_back(plugin);
            }
        }
    }

    // subcommands セクション
    if (const auto *subs = tbl["subcommands"].as_table()) {
        for (std::size_t i = 0; i < kSubcommandMappingCount; ++i) {
            const auto &mapping = kSubcommandMappings[i];
            if (const auto *sub_tbl = (*subs)[mapping.key].as_table()) {
                (conf.*mapping.member).a = (*sub_tbl)["a"].value_or(0);
                (conf.*mapping.member).b = (*sub_tbl)["b"].value_or(0);
            }
        }
    }
}

// ──────────────────────────────────────────────
// JSON/JSONC ユーティリティ
// ──────────────────────────────────────────────

template <typename T>
std::optional<T> ResolveJsonKey(const nlohmann::json &j, std::string_view dotted_key) {
    const nlohmann::json *current = &j;
    std::string_view remaining = dotted_key;

    while (true) {
        const auto dot_pos = remaining.find('.');
        if (dot_pos == std::string_view::npos) {
            const auto key = std::string(remaining);
            if (!current->is_object() || !current->contains(key)) {
                return std::nullopt;
            }
            return current->at(key).get<T>();
        }
        const auto head = std::string(remaining.substr(0, dot_pos));
        remaining = remaining.substr(dot_pos + 1);
        if (!current->is_object() || !current->contains(head) || !current->at(head).is_object()) {
            return std::nullopt;
        }
        current = &current->at(head);
    }
}

void LoadFromJson(const std::string &file_path, Config &conf) {
    std::ifstream ifs(file_path);
    if (!ifs) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }
    const auto j = nlohmann::json::parse(
        ifs, /*callback cb=*/nullptr,
        /*allow_exceptions=*/true,
        /*ignore_comments=*/true
    );

    // スキーマフィールドを自動マッピング
    std::apply(
        [&](auto &&...field) {
            (
                [&] {
                    using FieldType = std::remove_reference_t<decltype(conf.*field.member)>;
                    auto val = ResolveJsonKey<FieldType>(j, field.config_key);
                    if (val.has_value()) {
                        conf.*field.member = *val;
                    }
                }(),
                ...
            );
        },
        kConfigSchema
    );

    // plugins 配列
    if (j.contains("plugin") && j.at("plugin").is_array()) {
        conf.plugins.clear();
        for (const auto &el : j.at("plugin")) {
            PluginConfig plugin;
            if (el.contains("file") && el.at("file").is_string()) {
                plugin.file = el.at("file").get<std::string>();
            }
            if (el.contains("number") && el.at("number").is_number_unsigned()) {
                plugin.number = el.at("number").get<std::uint64_t>();
            }
            conf.plugins.push_back(plugin);
        }
    }
}

// ──────────────────────────────────────────────
// YAML ユーティリティ
// ──────────────────────────────────────────────

template <typename T>
std::optional<T> ResolveYamlKey(const fkyaml::node &node, std::string_view dotted_key) {
    const fkyaml::node *current = &node;
    std::string_view remaining = dotted_key;

    while (true) {
        const auto dot_pos = remaining.find('.');
        if (dot_pos == std::string_view::npos) {
            const auto key = std::string(remaining);
            if (!current->is_mapping() || !current->contains(key)) {
                return std::nullopt;
            }
            return current->at(key).get_value<T>();
        }
        const auto head = std::string(remaining.substr(0, dot_pos));
        remaining = remaining.substr(dot_pos + 1);
        if (!current->is_mapping() || !current->contains(head) || !current->at(head).is_mapping()) {
            return std::nullopt;
        }
        current = &current->at(head);
    }
}

void LoadFromYaml(const std::string &file_path, Config &conf) {
    std::ifstream ifs(file_path);
    if (!ifs) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }
    const auto root = fkyaml::node::deserialize(ifs);

    // スキーマフィールドを自動マッピング
    std::apply(
        [&](auto &&...field) {
            (
                [&] {
                    using FieldType = std::remove_reference_t<decltype(conf.*field.member)>;
                    auto val = ResolveYamlKey<FieldType>(root, field.config_key);
                    if (val.has_value()) {
                        conf.*field.member = *val;
                    }
                }(),
                ...
            );
        },
        kConfigSchema
    );

    // plugins 配列
    const auto key = std::string("plugin");
    if (root.is_mapping() && root.contains(key) && root.at(key).is_sequence()) {
        conf.plugins.clear();
        for (const auto &el : root.at(key)) {
            PluginConfig plugin;
            if (el.is_mapping() && el.contains("file")) {
                plugin.file = el.at("file").get_value<std::string>();
            }
            if (el.is_mapping() && el.contains("number")) {
                plugin.number = el.at("number").get_value<std::uint64_t>();
            }
            conf.plugins.push_back(plugin);
        }
    }
}

} // namespace

// ──────────────────────────────────────────────
// 公開API
// ──────────────────────────────────────────────

void LoadFromFile(const std::string &file_path, Config &conf) {
    const auto ext = std::filesystem::path(file_path).extension().string();
    if (ext == ".toml") {
        LoadFromToml(file_path, conf);
    } else if (ext == ".json") {
        LoadFromJson(file_path, conf);
    } else if (ext == ".yaml" || ext == ".yml") {
        LoadFromYaml(file_path, conf);
    } else {
        throw std::runtime_error("Unsupported config file extension: " + ext);
    }
}

std::string FindDefaultConfig() {
    const std::vector<std::string> candidates = {
        "config/default.toml",
        "config/default.json",
        "config/default.yaml",
    };

    std::vector<std::string> found;
    std::copy_if(candidates.begin(), candidates.end(), std::back_inserter(found), [](const std::string &path) {
        return std::filesystem::exists(path);
    });

    if (found.size() > 1) {
        std::string msg = "Multiple default config files found:";
        for (const auto &p : found) {
            msg += " " + p;
        }
        throw std::runtime_error(msg);
    }

    return found.empty() ? std::string{} : found.front();
}

} // namespace config
