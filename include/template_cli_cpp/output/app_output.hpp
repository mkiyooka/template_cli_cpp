#pragma once

#include "template_cli_cpp/logging/logger.hpp"
#include "template_cli_cpp/recording/recorder_manager.hpp"

/**
 * @brief アプリケーション出力の統合 DI コンテナ
 *
 * 診断ログ（Logger）と解析データ（RecorderManager）を一つにまとめ、
 * アプリケーションコアに DI で注入する。
 *
 * @tparam Key RecorderManager のキー型（enum class 等）
 *
 * @code
 * enum class Module { X, Y };
 *
 * AppOutput<Module> out(logger, recorder_manager);
 *
 * void run(AppOutput<Module>& out) {
 *     out.logger().log(LogLevel::Debug, "initialize start");
 *     out.recorders()[Module::X].enable();
 *     out.recorders()[Module::X].write("{},{:.6f}", step, value);
 * }
 * @endcode
 */
template <typename Key>
class AppOutput {
public:
    AppOutput(Logger& logger, RecorderManager<Key>& recorders)
        : logger_(logger), recorders_(recorders) {}

    /**
     * @brief 診断ロガーへの参照を返す
     */
    Logger& logger() { return logger_; }

    /**
     * @brief レコーダーマネージャーへの参照を返す
     */
    RecorderManager<Key>& recorders() { return recorders_; }

private:
    Logger& logger_;
    RecorderManager<Key>& recorders_;
};
