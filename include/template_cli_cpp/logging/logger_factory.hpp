#pragma once

#include <memory>
#include <string>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "template_cli_cpp/logging/logger.hpp"
#include "template_cli_cpp/logging/null_logger.hpp"
#include "template_cli_cpp/logging/spdlog_logger.hpp"

/**
 * @brief Logger インスタンスを生成するファクトリ
 *
 * 出力先・同期/非同期の選択を一箇所に集約し、呼び出し側が
 * spdlog の詳細を知らなくてもよくする。
 *
 * @code
 * auto logger = LoggerFactory::MakeFile("app", "app.log", LogLevel::Info);
 * @endcode
 */
struct LoggerFactory {
    /**
     * @brief 標準出力（カラー付き）に書き込む同期ロガーを生成する
     * @param name   spdlog 内部名（重複不可）
     * @param level  初期ログレベル
     */
    static std::unique_ptr<Logger> MakeConsole(const std::string &name, LogLevel level = LogLevel::Info) {
        auto inner = spdlog::stdout_color_mt(name);
        auto logger = std::make_unique<SpdlogLogger>(inner);
        logger->set_level(level);
        return logger;
    }

    /**
     * @brief ファイルに書き込む同期ロガーを生成する
     * @param name      spdlog 内部名（重複不可）
     * @param file_path 出力ファイルパス
     * @param level     初期ログレベル
     */
    static std::unique_ptr<Logger>
    MakeFile(const std::string &name, const std::string &file_path, LogLevel level = LogLevel::Info) {
        auto inner = spdlog::basic_logger_mt(name, file_path);
        auto logger = std::make_unique<SpdlogLogger>(inner);
        logger->set_level(level);
        return logger;
    }

    /**
     * @brief 何も出力しないロガーを生成する（テスト・無効化用）
     */
    static std::unique_ptr<Logger> MakeNull() { return std::make_unique<NullLogger>(); }
};
