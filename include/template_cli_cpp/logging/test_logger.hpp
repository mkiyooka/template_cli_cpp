#pragma once

#include <string>
#include <vector>

#include "template_cli_cpp/logging/logger.hpp"

/**
 * @brief テスト用ロガー
 *
 * ログエントリをメモリに蓄積し、テストコードから検証できる。
 *
 * @code
 * TestLogger logger;
 * process(logger);
 * assert(logger.entries().size() == 3);
 * @endcode
 */
class TestLogger : public Logger {
public:
    void Log(LogLevel lvl, std::string_view msg) override {
        if (lvl >= level_) {
            entries_.emplace_back(msg);
        }
    }

    void set_level(LogLevel lvl) override { level_ = lvl; }

    LogLevel level() const override { return level_; }

    /**
     * @brief 蓄積されたログエントリを返す
     */
    const std::vector<std::string>& entries() const { return entries_; }

    /**
     * @brief 蓄積されたログエントリをクリアする
     */
    void clear() { entries_.clear(); }

private:
    LogLevel level_ = LogLevel::Trace;
    std::vector<std::string> entries_;
};
