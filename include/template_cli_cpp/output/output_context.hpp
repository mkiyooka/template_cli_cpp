#pragma once

#include "template_cli_cpp/logging/logger.hpp"
#include "template_cli_cpp/recording/recorder_manager.hpp"

/**
 * @brief 出力処理に必要な依存をまとめた DI コンテキスト
 *
 * 診断ログ（Logger）と解析データ（RecorderManager）を一つにまとめ、
 * アプリケーションコアに DI で注入する。
 *
 * @tparam Key RecorderManager のキー型（enum class 等）
 *
 * @code
 * enum class Module { X, Y };
 *
 * OutputContext<Module> output_context(logger, recorder_manager);
 *
 * void run(OutputContext<Module>& output_context) {
 *     output_context.GetLogger().Log(LogLevel::Debug, "initialize start");
 *     output_context.GetRecorders()[Module::X].Enable();
 *     output_context.GetRecorders()[Module::X].Write("{},{:.6f}", step, value);
 * }
 * @endcode
 */
template <typename Key>
class OutputContext {
public:
    OutputContext(Logger &logger, RecorderManager<Key> &recorders)
        : logger_(&logger),
          recorders_(&recorders) {}

    /**
     * @brief 診断ロガーへの参照を返す
     */
    Logger &GetLogger() { return *logger_; }

    /**
     * @brief レコーダーマネージャーへの参照を返す
     */
    RecorderManager<Key> &GetRecorders() { return *recorders_; }

private:
    Logger *logger_;
    RecorderManager<Key> *recorders_;
};
