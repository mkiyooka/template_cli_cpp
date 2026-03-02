#pragma once

#include <string_view>

#include <fmt/format.h>

/**
 * @brief 解析データ記録の共通インターフェース
 *
 * モジュール別にデータ出力の有効・無効を制御し、
 * 出力先（ファイル・メモリ等）を DI で切り替えられる。
 *
 * フォーマットは呼び出し側で行い、文字列として渡す設計とする。
 * ただし write() テンプレートヘルパーで fmt::format の利便性を維持する。
 *
 * @code
 * recorder.write("{},{:.6f}", step, value);
 * @endcode
 */
class DataRecorder {
public:
    virtual ~DataRecorder() = default;

    /**
     * @brief 出力を有効化する
     */
    virtual void enable() = 0;

    /**
     * @brief 出力を無効化する
     */
    virtual void disable() = 0;

    /**
     * @brief 出力が有効かを返す
     */
    virtual bool is_enabled() const = 0;

    /**
     * @brief 文字列をそのまま出力する
     * @param msg 出力するメッセージ
     */
    virtual void output(std::string_view msg) = 0;

    /**
     * @brief 出力バッファをフラッシュする
     */
    virtual void flush() = 0;

    /**
     * @brief fmt::format でフォーマットしてから output() に渡す非仮想ヘルパー
     *
     * フォーマット文字列はコンパイル時にチェックされる。
     * is_enabled() が false の場合はフォーマット処理自体をスキップする。
     *
     * @code
     * recorder.write("{},{:.6f}", step, value);
     * @endcode
     */
    template <typename... Args>
    void write(fmt::format_string<Args...> fmt_str, Args&&... args) {
        if (is_enabled()) {
            output(fmt::format(fmt_str, std::forward<Args>(args)...));
        }
    }
};
