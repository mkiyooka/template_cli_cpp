# CMake version requirements:
# - cmake --preset: >= 3.19
# - project requirement: >= 3.19
cmake_minimum_required(VERSION 3.19)

# Set default build type
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting default build type to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Default compiler preference (non-macOS: GCC preferred)
if(NOT APPLE AND NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(GCC_C_COMPILER NAMES gcc cc)
    find_program(GCC_CXX_COMPILER NAMES g++ c++)
    if(GCC_C_COMPILER AND GCC_CXX_COMPILER)
        set(CMAKE_C_COMPILER "${GCC_C_COMPILER}" CACHE FILEPATH "C compiler" FORCE)
        set(CMAKE_CXX_COMPILER "${GCC_CXX_COMPILER}" CACHE FILEPATH "C++ compiler" FORCE)
        message(STATUS "Using GCC toolchain: ${GCC_C_COMPILER}, ${GCC_CXX_COMPILER}")
    endif()
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Enable position-independent code. add "-fPIC"

project(template_cli_cpp
    VERSION 0.1.0
    LANGUAGES CXX C
)

# compile_commands.json 生成とプロジェクトルートディレクトリへの自動配置
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${CMAKE_BINARY_DIR}/compile_commands.json 
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    COMMENT "Copying compile_commands.json to project root"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable プリコンパイルドヘッダー (optional, for faster compilation)
option(USE_PCH "Use precompiled headers" OFF)

include_directories(include)

# FetchContentで導入するライブラリ
include(FetchContent)
include(cmake/local-or-fetch.cmake)

# CLI11 - Command line parser
declare_fetchcontent_with_local(CLI11 ext/CLI11-2.5.0
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.5.0
)
FetchContent_MakeAvailable(CLI11)

# fmt - Formatting library
declare_fetchcontent_with_local(fmt ext/fmt-11.2.0
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0
)
FetchContent_MakeAvailable(fmt)

# tomlplusplus - TOML configuration library
declare_fetchcontent_with_local(tomlplusplus ext/tomlplusplus-3.4.0
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

# doctest - Testing framework
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
declare_fetchcontent_with_local(doctest ext/doctest-2.4.11
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

# argparse libraries (header-only)
# morrisfranken/argparse - use header-only approach
declare_fetchcontent_with_local(argparse_morris ext/argparse_morris-master
    GIT_REPOSITORY https://github.com/morrisfranken/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparse_morris)

# Create interface library for morris version
add_library(argparse_morris_target INTERFACE)
target_include_directories(argparse_morris_target INTERFACE ${argparse_morris_SOURCE_DIR}/include)

# ==============================================================================
# Main Application - メインCLIアプリケーション
# ==============================================================================

# メインのソースディレクトリ
add_subdirectory(src)

# Add debug flags for Debug and RelWithDebInfo builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
    message(STATUS "Debug symbols enabled for CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -O2")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -O2")
    message(STATUS "Release with debug symbols enabled for CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()

# Development Environment Setup
# Enable testing for CTest
enable_testing()

# ==============================================================================
# Development utilities
# ==============================================================================

add_subdirectory(tests/cpp)

# cmake --build build --target test テスト実行
# cmake --build build --target run-tests テスト実行 + 失敗時に詳細出力
add_custom_target(run-tests
    COMMAND ctest --output-on-failure
    COMMENT "Running all tests"
)

# ヘルプターゲット
add_custom_target(show-help
    COMMAND ${CMAKE_COMMAND} -E echo "=== Available Build Targets ==="
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Main application:"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmd_cli11             - Main CLI application"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Development targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  run-tests             - Run all tests"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Code quality targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  format                - Format code with clang-format"
    COMMAND ${CMAKE_COMMAND} -E echo "  format-dry            - Check formatting without changes"
    COMMAND ${CMAKE_COMMAND} -E echo "  lint                  - Lint code with clang-tidy"
    COMMAND ${CMAKE_COMMAND} -E echo "  run-cppcheck          - Run cppcheck static analysis"
    COMMAND ${CMAKE_COMMAND} -E echo "  run-cppcheck-verbose  - Run cppcheck with verbose output"
    COMMENT "Showing available build targets"
)

# Collect source files for quality tools
file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.c
    ${CMAKE_SOURCE_DIR}/debug/**/*.cpp
    ${CMAKE_SOURCE_DIR}/debug/**/*.c
    ${CMAKE_SOURCE_DIR}/tests/**/*.cpp
    ${CMAKE_SOURCE_DIR}/include/**/*.hpp
    ${CMAKE_SOURCE_DIR}/include/**/*.h
)

# Separate source files for clang-tidy (only compilable sources)
file(GLOB_RECURSE COMPILABLE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.c
    ${CMAKE_SOURCE_DIR}/debug/**/*.cpp
    ${CMAKE_SOURCE_DIR}/debug/**/*.c
    ${CMAKE_SOURCE_DIR}/tests/**/*.cpp
)

# Remove ext/ directory files from quality tools processing
file(GLOB_RECURSE EXT_FILES ${CMAKE_SOURCE_DIR}/ext/**)
list(REMOVE_ITEM ALL_SOURCE_FILES ${EXT_FILES})
list(REMOVE_ITEM COMPILABLE_SOURCE_FILES ${EXT_FILES})

# Setup quality management tools
include(cmake/quality-tools.cmake)
setup_quality_tools()
setup_quality_targets("${ALL_SOURCE_FILES}" "${COMPILABLE_SOURCE_FILES}")

# ==============================================================================
# Third-Party License Aggregation
# ==============================================================================

# サードパーティライブラリのライセンス集約
add_custom_target(collect-licenses
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/third_party_licenses"

    # Conanでインストールされたライブラリのライセンス
    COMMAND ${CMAKE_COMMAND} -E echo "Collecting Conan package licenses..."
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/collect-conan-licenses.cmake

    # FetchContentで取得したライブラリのライセンス
    COMMAND ${CMAKE_COMMAND} -E echo "Collecting FetchContent package licenses..."
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/collect-fetchcontent-licenses.cmake

    COMMENT "Collecting third-party library licenses"
)

