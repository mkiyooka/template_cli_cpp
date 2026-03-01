#pragma once

#include <string>
#include <vector>

#include <CLI/CLI.hpp>

#include "config/config_loader.hpp"

namespace config {

/**
 * @brief CLIオプションと設定ファイルを統合管理するクラス
 *
 * CLI11への全オプション登録と、設定ファイル読み込みを担い、
 * 優先度 (CLI引数 > 設定ファイル > デフォルト値) に従って最終的な Config を生成する。
 *
 * 使い方:
 * @code
 * ConfigManager mgr;
 * std::string config_path;
 * app.add_option("-c,--config", config_path, "Configuration file");
 * mgr.RegisterOptions(app);   // スキーマ由来の全オプションを登録
 * app.parse(argc, argv);      // CLI11がパース
 * Config conf = mgr.Resolve(config_path);  // 優先度解決
 * @endcode
 */
class ConfigManager {
public:
    ConfigManager();

    /**
     * @brief スキーマに基づいてCLI::Appにオプションを登録する
     * @param app CLI11のアプリケーションオブジェクト
     */
    void RegisterOptions(CLI::App &app);

    /**
     * @brief 設定を優先度に従って解決して返す
     *
     * @param explicit_config_path -c/--config で指定されたパス (空文字列の場合はデフォルト探索)
     * @return 解決済みのConfig
     * @throws std::runtime_error 複数のデフォルト設定ファイルが存在する場合
     * @throws std::runtime_error 設定ファイルのパースに失敗した場合
     */
    Config Resolve(const std::string &explicit_config_path);

private:
    Config cli_values_;         ///< CLI11のパース結果書き込み先
    std::vector<bool> cli_set_; ///< 各スキーマフィールドがCLIで明示指定されたか
};

/**
 * @brief Configの内容を標準出力に表示する
 * @param conf 表示するConfig
 */
void ShowConfig(const Config &conf);

} // namespace config
