#pragma once

#include <memory>
#include <string>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "template_cli_cpp/recording/data_recorder.hpp"
#include "template_cli_cpp/recording/null_recorder.hpp"
#include "template_cli_cpp/recording/spdlog_recorder.hpp"

/**
 * @brief DataRecorder インスタンスを生成するファクトリ
 *
 * 出力先の選択を一箇所に集約し、呼び出し側が
 * spdlog の詳細を知らなくてもよくする。
 *
 * @code
 * auto rec = RecorderFactory::MakeFile("moduleX", "moduleX.csv");
 * rec->Enable();
 * @endcode
 */
struct RecorderFactory {
    /**
     * @brief ファイルに書き込む同期レコーダーを生成する
     *
     * spdlog のパターンを "%v"（メッセージのみ）に設定する。
     * 初期状態は disabled。
     *
     * @param name      spdlog 内部名（重複不可）
     * @param file_path 出力ファイルパス
     */
    static std::unique_ptr<DataRecorder> MakeFile(const std::string &name, const std::string &file_path) {
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path);
        auto inner = std::make_shared<spdlog::logger>(name, sink);
        return std::make_unique<SpdlogRecorder>(inner);
    }

    /**
     * @brief 標準出力（カラー付き）に書き込む同期レコーダーを生成する
     *
     * @param name spdlog 内部名（重複不可）
     */
    static std::unique_ptr<DataRecorder> MakeConsole(const std::string &name) {
        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto inner = std::make_shared<spdlog::logger>(name, sink);
        return std::make_unique<SpdlogRecorder>(inner);
    }

    /**
     * @brief 何も出力しないレコーダーを生成する（テスト・無効化用）
     */
    static std::unique_ptr<DataRecorder> MakeNull() { return std::make_unique<NullRecorder>(); }
};
