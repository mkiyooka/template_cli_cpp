#pragma once

#include "template_cli_cpp/logging/logger.hpp"

/**
 * @brief 何も出力しないロガー
 *
 * ロギングを無効化したい場合や、DI先のデフォルト実装として使用する。
 */
class NullLogger : public Logger {
public:
    void Log(LogLevel, std::string_view) override {}
    void set_level(LogLevel) override {}
    LogLevel level() const override { return LogLevel::Off; }
};
