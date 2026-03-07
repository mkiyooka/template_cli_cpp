#include <cstddef>
#include <string>
#include <tuple>

#include "config_file_loader.hpp"
#include "config/config_manager.hpp"
#include "config/config_schema.hpp"

namespace config {

namespace {

constexpr std::size_t kSchemaSize = std::tuple_size_v<decltype(kConfigSchema)>;

} // namespace

ConfigManager::ConfigManager()
    : cli_values_{},
      file_values_{},
      cli_set_(kSchemaSize, false) {}

void ConfigManager::RegisterOptions(CLI::App &app) {
    std::size_t idx = 0;
    std::apply(
        [&](auto &&...field) {
            (
                [&] {
                    auto *opt = app.add_option(
                        std::string(field.cli_option), cli_values_.*field.member, std::string(field.description)
                    );
                    const std::size_t i = idx++;
                    opt->each([this, i](const std::string & /*unused*/) { cli_set_[i] = true; });
                }(),
                ...
            );
        },
        kConfigSchema
    );
}

Config ConfigManager::Resolve(const std::string &explicit_config_path) {
    // デフォルト値で初期化
    Config result{};

    // 設定ファイルを読み込む
    file_values_ = Config{};
    const std::string config_path = explicit_config_path.empty() ? FindDefaultConfig() : explicit_config_path;
    if (!config_path.empty()) {
        LoadFromFile(config_path, file_values_);
    }

    // スキーマフィールドをファイル値で上書き (デフォルト < ファイル)
    std::apply([&](auto &&...field) { ((result.*field.member = file_values_.*field.member), ...); }, kConfigSchema);

    // CLI値で上書き (ファイル < CLI)
    std::size_t idx = 0;
    std::apply(
        [&](auto &&...field) {
            (
                [&] {
                    if (cli_set_[idx++]) {
                        result.*field.member = cli_values_.*field.member;
                    }
                }(),
                ...
            );
        },
        kConfigSchema
    );

    return result;
}

} // namespace config
