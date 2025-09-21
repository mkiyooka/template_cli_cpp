#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <yyjson.h>

namespace json {

/**
 * @brief yyjsonラッパー - 高性能JSON構築ライブラリ
 *
 * 特徴:
 * - 統一Add関数による直感的API
 * - コンパイル時型推論で最適化
 * - 高性能（約300ns/op）
 * - メモリ安全（RAII、copy API使用）
 *
 * 使用例:
 * @code
 * json::JsonBuilder builder;
 * builder.Add("name", "John");
 * builder.Add("age", 30);
 * builder.Add("scores", std::vector<int>{95, 87, 92});
 * std::string json = builder.Serialize();
 * @endcode
 */
class JsonBuilder {
public:
    /**
     * @brief コンストラクタ - JSONドキュメントを初期化
     * @throws std::runtime_error yyjsonドキュメント作成に失敗した場合
     */
    JsonBuilder()
        : doc_(yyjson_mut_doc_new(nullptr)),
          root_(nullptr) {
        if (doc_ == nullptr) {
            throw std::runtime_error("Failed to create yyjson document");
        }
        root_ = yyjson_mut_obj(doc_);
        if (root_ == nullptr) {
            yyjson_mut_doc_free(doc_);
            throw std::runtime_error("Failed to create root object");
        }
        yyjson_mut_doc_set_root(doc_, root_);
    }

    // コピー禁止、ムーブ許可（RAII設計）
    JsonBuilder(const JsonBuilder &) = delete;
    JsonBuilder &operator=(const JsonBuilder &) = delete;

    JsonBuilder(JsonBuilder &&other) noexcept
        : doc_(other.doc_),
          root_(other.root_) {
        other.doc_ = nullptr;
        other.root_ = nullptr;
    }

    JsonBuilder &operator=(JsonBuilder &&other) noexcept {
        if (this != &other) {
            if (doc_ != nullptr) {
                yyjson_mut_doc_free(doc_);
            }
            doc_ = other.doc_;
            root_ = other.root_;
            other.doc_ = nullptr;
            other.root_ = nullptr;
        }
        return *this;
    }

    /**
     * @brief デストラクタ - リソースを自動解放
     */
    ~JsonBuilder() {
        if (doc_ != nullptr) {
            yyjson_mut_doc_free(doc_);
        }
    }

    /**
     * @brief 統一Add関数 - あらゆる型をサポート
     *
     * サポート型:
     * - int, double, float: 数値
     * - bool: 真偽値
     * - std::string, const char*: 文字列
     * - std::vector<int>: 数値配列
     * - std::vector<std::string>: 文字列配列
     *
     * @tparam T 値の型（自動推論）
     * @param key JSONキー名
     * @param value 設定する値
     *
     * @code
     * builder.Add("count", 42);              // int
     * builder.Add("price", 99.99);           // double
     * builder.Add("name", "Product");        // string
     * builder.Add("active", true);           // bool
     * builder.Add("tags", std::vector<std::string>{"new", "sale"});
     * @endcode
     */
    template <typename T>
    void Add(const char *key, T &&value) {
        if constexpr (std::is_same_v<std::decay_t<T>, int>) {
            yyjson_mut_obj_add_int(doc_, root_, key, value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, double> || std::is_same_v<std::decay_t<T>, float>) {
            yyjson_mut_obj_add_real(doc_, root_, key, static_cast<double>(value));
        } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            yyjson_mut_obj_add_bool(doc_, root_, key, value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
            yyjson_mut_obj_add_strcpy(doc_, root_, key, value.c_str());
        } else if constexpr (std::is_same_v<std::decay_t<T>, const char *> || std::is_same_v<std::decay_t<T>, char *>) {
            yyjson_mut_obj_add_strcpy(doc_, root_, key, value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, std::vector<int>>) {
            AddIntArray(key, value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, std::vector<std::string>>) {
            AddStringArray(key, value);
        } else {
            static_assert(std::is_same_v<T, void>, "Unsupported type for Add function");
        }
    }

    /**
     * @brief ネストしたオブジェクトを作成
     * @param key ネストオブジェクトのキー名
     * @return ネストオブジェクトのポインタ（AddToNestedで使用）
     *
     * @code
     * auto* user = builder.AddNested("user");
     * builder.AddToNested(user, "name", "John");
     * builder.AddToNested(user, "age", 30);
     * @endcode
     */
    yyjson_mut_val *AddNested(const char *key) {
        yyjson_mut_val *nested_obj = yyjson_mut_obj(doc_);
        yyjson_mut_obj_add_val(doc_, root_, key, nested_obj);
        return nested_obj;
    }

    /**
     * @brief ネストオブジェクトに値を追加
     * @tparam T 値の型（自動推論）
     * @param nested_obj AddNestedで取得したネストオブジェクト
     * @param key キー名
     * @param value 設定する値
     */
    template <typename T>
    void AddToNested(yyjson_mut_val *nested_obj, const char *key, T &&value) {
        if constexpr (std::is_same_v<std::decay_t<T>, int>) {
            yyjson_mut_obj_add_int(doc_, nested_obj, key, value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, double> || std::is_same_v<std::decay_t<T>, float>) {
            yyjson_mut_obj_add_real(doc_, nested_obj, key, static_cast<double>(value));
        } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            yyjson_mut_obj_add_bool(doc_, nested_obj, key, value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
            yyjson_mut_obj_add_strcpy(doc_, nested_obj, key, value.c_str());
        } else if constexpr (std::is_same_v<std::decay_t<T>, const char *> || std::is_same_v<std::decay_t<T>, char *>) {
            yyjson_mut_obj_add_strcpy(doc_, nested_obj, key, value);
        } else {
            static_assert(std::is_same_v<T, void>, "Unsupported type for AddToNested function");
        }
    }

    /**
     * @brief JSONデータの文字列表現を取得
     * @param pretty フォーマットするか（デフォルト: false）
     * @return JSON文字列
     *
     * @code
     * std::string compact = builder.Serialize();
     * std::string formatted = builder.Serialize(true);
     * @endcode
     */
    std::string Serialize(bool pretty = false) const {
        size_t len = 0;
        uint32_t flags = pretty ? YYJSON_WRITE_PRETTY : YYJSON_WRITE_NOFLAG;
        char *json_str = yyjson_mut_write(doc_, flags, &len);
        if (json_str == nullptr) {
            return "{}";
        }
        std::string result(json_str, len);
        free(json_str);
        return result;
    }

    /**
     * @brief 現在のJSONサイズ（フィールド数）を取得
     * @return フィールド数
     */
    size_t Size() const { return yyjson_mut_obj_size(root_); }

    /**
     * @brief JSONが空かどうかチェック
     * @return 空の場合true
     */
    bool Empty() const { return Size() == 0; }

    /**
     * @brief JSONをクリア（全フィールドを削除）
     */
    void Clear() {
        yyjson_mut_doc_free(doc_);
        doc_ = yyjson_mut_doc_new(nullptr);
        if (doc_ == nullptr) {
            throw std::runtime_error("Failed to recreate yyjson document");
        }
        root_ = yyjson_mut_obj(doc_);
        if (root_ == nullptr) {
            yyjson_mut_doc_free(doc_);
            throw std::runtime_error("Failed to recreate root object");
        }
        yyjson_mut_doc_set_root(doc_, root_);
    }

private:
    yyjson_mut_doc *doc_{};
    yyjson_mut_val *root_{};

    // プライベートヘルパー関数
    void AddIntArray(const char *key, const std::vector<int> &arr) {
        yyjson_mut_val *json_arr = yyjson_mut_arr(doc_);
        for (int item : arr) {
            yyjson_mut_arr_add_int(doc_, json_arr, item);
        }
        yyjson_mut_obj_add_val(doc_, root_, key, json_arr);
    }

    void AddStringArray(const char *key, const std::vector<std::string> &arr) {
        yyjson_mut_val *json_arr = yyjson_mut_arr(doc_);
        for (const auto &item : arr) {
            yyjson_mut_arr_add_strcpy(doc_, json_arr, item.c_str());
        }
        yyjson_mut_obj_add_val(doc_, root_, key, json_arr);
    }
};

} // namespace json
