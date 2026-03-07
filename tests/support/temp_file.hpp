#pragma once

#include <filesystem>
#include <fstream>
#include <string>

/**
 * @brief テスト用の一時ファイルを作成するヘルパー
 *
 * コンストラクタでファイルを生成し、デストラクタで自動削除する。
 *
 * @code
 * const TempFile tmp("test.toml", "[settings]\nvalue = 42\n");
 * config::LoadFromFile(tmp.Str(), conf);
 * @endcode
 */
struct TempFile {
    std::filesystem::path path;

    TempFile(const std::string &name, const std::string &content)
        : path(std::filesystem::temp_directory_path() / name) {
        std::ofstream ofs(path);
        ofs << content;
    }

    ~TempFile() { std::filesystem::remove(path); }

    std::string Str() const { return path.string(); }
};
