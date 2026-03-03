#pragma once

#include <string_view>

/**
 * @brief ログレベル定義
 */
enum class LogLevel : int { Trace = 0, Debug, Info, Warn, Error, Critical, Off };

/**
 * @brief ロガーの共通インターフェース
 *
 * DI（依存性注入）により呼び出し側をロガー実装から分離する。
 * 実運用では SpdlogLogger を、テストでは NullLogger を注入する。
 */
class Logger {
public:
    virtual ~Logger() = default;

    /**
     * @brief メッセージをログに出力する
     * @param level ログレベル
     * @param msg   出力メッセージ
     */
    virtual void Log(LogLevel level, std::string_view msg) = 0;

    /**
     * @brief 最小ログレベルを設定する
     */
    virtual void set_level(LogLevel level) = 0;

    /**
     * @brief 現在のログレベルを返す
     */
    virtual LogLevel level() const = 0;

    /**
     * @brief 指定レベルが出力対象かを返す
     *
     * コストの高い文字列生成を出力が確定した場合だけ行うために使う。
     * @code
     * if (logger.ShouldLog(LogLevel::Debug)) {
     *     logger.Log(LogLevel::Debug, expensive_to_string());
     * }
     * @endcode
     */
    bool ShouldLog(LogLevel lvl) const { return lvl >= level(); }
};
