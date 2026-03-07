#pragma once

#include <string>

#include "config/config_loader.hpp"

namespace config {

/**
 * @brief 設定ファイルを読み込んでConfigに書き込む
 *
 * ファイル拡張子から形式を自動判別する。
 * スキーマ (config_schema.hpp) のconfig_keyに従って自動マッピングする。
 * ファイルに存在するキーのみConfigフィールドを上書きし、存在しないキーはそのまま。
 *
 * @param file_path 読み込むファイルパス (.toml / .json / .yaml / .yml)
 * @param conf 書き込み先のConfig
 * @throws std::runtime_error ファイルが開けない場合またはパース失敗時
 * @throws std::runtime_error 未対応の拡張子の場合
 */
void LoadFromFile(const std::string &file_path, Config &conf);

/**
 * @brief デフォルト設定ファイルを探索して返す
 *
 * 探索パス: config/default.toml, config/default.json, config/default.yaml
 * 複数同時に存在する場合はエラー。
 *
 * @return 見つかったファイルパス。見つからなければ空文字列。
 * @throws std::runtime_error 複数のデフォルト設定ファイルが同時に存在する場合
 */
std::string FindDefaultConfig();

} // namespace config
