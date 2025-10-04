# メインアプリケーション用のサードパーティライブラリ定義
#
# 使用例:
#   declare_fetchcontent_with_local(fmt third_party/fmt-11.2.0
#       GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#       GIT_TAG 11.2.0
#   )
#   FetchContent_MakeAvailable(fmt)

# CLI11 - Command line parser
declare_fetchcontent_with_local(CLI11 third_party/CLI11-2.5.0
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.5.0
)
FetchContent_MakeAvailable(CLI11)

# fmt - Formatting library
declare_fetchcontent_with_local(fmt third_party/fmt-11.2.0
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0
)
FetchContent_MakeAvailable(fmt)

# tomlplusplus - TOML configuration library
declare_fetchcontent_with_local(tomlplusplus third_party/tomlplusplus-3.4.0
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

# argparse libraries (header-only)
# morrisfranken/argparse - use header-only approach
declare_fetchcontent_with_local(argparse_morris third_party/argparse_morris-master
    GIT_REPOSITORY https://github.com/morrisfranken/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparse_morris)

# Create interface library for morris version
add_library(argparse_morris_target INTERFACE)
target_include_directories(argparse_morris_target INTERFACE ${argparse_morris_SOURCE_DIR}/include)
