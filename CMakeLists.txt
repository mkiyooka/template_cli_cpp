# CMake version requirements:
# - cmake --preset: >= 3.19
# - project requirement: >= 3.19
cmake_minimum_required(VERSION 3.19)

# Set default build type before Conan setup
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting default build type to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Conan setup
include(${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake OPTIONAL)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/generators)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/generators)

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
option(USE_PCH "Use precompiled headers" ON)

include_directories(include)

# Conanで導入されるサードパーティライブラリの検索
find_package(CLI11 REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(fmt REQUIRED)
find_package(doctest REQUIRED)

# FetchContentで導入するライブラリ
include(FetchContent)

# argparse libraries (header-only)
# morrisfranken/argparse - use header-only approach
FetchContent_Declare(
    argparse_morris
    GIT_REPOSITORY https://github.com/morrisfranken/argparse.git
    GIT_TAG master
)
FetchContent_Populate(argparse_morris)

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
    COMMAND ${CMAKE_COMMAND} -E echo "  fullcheck             - Run all quality checks"
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
setup_quality_check_target()
