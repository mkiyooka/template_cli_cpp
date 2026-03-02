#pragma once

#include "template_cli_cpp/recording/data_recorder.hpp"

/**
 * @brief 何も出力しないレコーダー
 *
 * 出力を完全に無効化したい場合や、DI先のデフォルト実装として使用する。
 */
class NullRecorder : public DataRecorder {
public:
    void enable() override {}
    void disable() override {}
    bool is_enabled() const override { return false; }
    void output(std::string_view) override {}
    void flush() override {}
};
