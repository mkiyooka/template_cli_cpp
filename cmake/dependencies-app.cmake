# メインアプリケーション用のサードパーティライブラリ定義
#
# 使用例:
#   add_external_package(fmt third_party/fmt-12.0.0
#       URL https://github.com/fmtlib/fmt/archive/refs/tags/12.0.0.tar.gz
#       URL_HASH SHA256=...
#   )
#   FetchContent_MakeAvailable(fmt)

# CLI11 - Command line parser
add_external_package(CLI11 third_party/CLI11-2.5.0
    URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.5.0.tar.gz
    URL_HASH SHA256=17e02b4cddc2fa348e5dbdbb582c59a3486fa2b2433e70a0c3bacb871334fd55
)
FetchContent_MakeAvailable(CLI11)

# fmt - Formatting library
add_external_package(fmt third_party/fmt-12.0.0
    URL https://github.com/fmtlib/fmt/archive/refs/tags/12.0.0.tar.gz
    URL_HASH SHA256=aa3e8fbb6a0066c03454434add1f1fc23299e85758ceec0d7d2d974431481e40
)
FetchContent_MakeAvailable(fmt)

# tomlplusplus - TOML configuration library
add_external_package(tomlplusplus third_party/tomlplusplus-3.4.0
    URL https://github.com/marzer/tomlplusplus/archive/refs/tags/v3.4.0.tar.gz
    URL_HASH SHA256=8517f65938a4faae9ccf8ebb36631a38c1cadfb5efa85d9a72e15b9e97d25155
)
FetchContent_MakeAvailable(tomlplusplus)

# nlohmann/json - JSON/JSONC parser
add_external_package(nlohmann_json third_party/nlohmann_json-3.12.0
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
)
FetchContent_MakeAvailable(nlohmann_json)

# fkYAML - YAML parser (header-only)
add_external_package(fkYAML third_party/fkYAML-0.4.2
    URL https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkYAML.tgz
    URL_HASH SHA256=6fbdbd94094b467ea37a64ccfe042588353feda097b1a5348f8cd12b914ad2cd
)
FetchContent_MakeAvailable(fkYAML)

# Create interface library for fkYAML (header-only)
add_library(fkYAML_target INTERFACE)
target_include_directories(fkYAML_target INTERFACE ${fkYAML_SOURCE_DIR}/include)

# argparse libraries (header-only)
# morrisfranken/argparse - use header-only approach
add_external_package(argparse_morris third_party/argparse_morris-master
    GIT_REPOSITORY https://github.com/morrisfranken/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparse_morris)

# Create interface library for morris version
add_library(argparse_morris_target INTERFACE)
target_include_directories(argparse_morris_target INTERFACE ${argparse_morris_SOURCE_DIR}/include)
