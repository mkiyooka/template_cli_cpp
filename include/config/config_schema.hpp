#pragma once

#include <string_view>
#include <tuple>

#include "config/config_loader.hpp"

namespace config {

/**
 * @brief Config構造体のフィールドに対するCLIオプションと設定ファイルキーのマッピング記述子
 *
 * @tparam Owner フィールドを持つ構造体型
 * @tparam T フィールドの型
 */
template <typename Owner, typename T>
struct FieldDescriptor {
    std::string_view cli_option;  ///< CLI11オプション名 (例: "--settings.value")
    std::string_view config_key;  ///< 設定ファイルキー (例: "settings.value", ドット区切りでネスト表現)
    std::string_view description; ///< CLIヘルプ文字列
    T Owner::*member;             ///< ポインタ・トゥ・メンバー
};

// CTAD補助 (C++17): FieldDescriptor<Owner, T>(...) の型推論を有効にする
template <typename Owner, typename T>
FieldDescriptor(std::string_view, std::string_view, std::string_view, T Owner::*) -> FieldDescriptor<Owner, T>;

/**
 * @brief Configのスキーマ定義
 *
 * 新しいオプションを追加するときはここに1行追加するだけでよい。
 * CLIオプション登録・設定ファイル読み込み・優先度解決はすべて自動化される。
 *
 * 追加例:
 * @code
 * FieldDescriptor{"--logging.level", "logging.level", "Log level (info/warn/error)", &Config::log_level},
 * @endcode
 */
inline constexpr auto kConfigSchema = std::make_tuple(
    FieldDescriptor{"--title", "title", "Application title", &Config::title},
    FieldDescriptor{"--settings.value", "settings.value", "Numeric value", &Config::value}
);

} // namespace config
