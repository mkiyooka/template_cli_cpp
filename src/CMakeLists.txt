# Add subdirectories
add_subdirectory(command)
add_subdirectory(config)

# CLI11 version - Main executable
add_executable(cmd_cli11 main_cli11.cpp)
target_link_libraries(cmd_cli11 PRIVATE command_lib config_lib)
target_link_libraries(cmd_cli11 PUBLIC fmt::fmt)
set_target_properties(cmd_cli11 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# morrisfranken argparse version - Alternative executable
add_executable(cmd_argparse_morris main_argparse_morris.cpp)
target_link_libraries(cmd_argparse_morris PRIVATE command_argparse_morris_lib config_lib)
target_link_libraries(cmd_argparse_morris PUBLIC fmt::fmt argparse_morris_target)
set_target_properties(cmd_argparse_morris PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Removed legacy cmd target that referenced non-existent main.cpp

# プリコンパイル済みヘッダーの設定
if(USE_PCH)
    target_precompile_headers(cmd_cli11 PRIVATE
        <memory>
        <string>
        <CLI/CLI.hpp>
        <fmt/base.h>
    )
    target_precompile_headers(cmd_argparse_morris PRIVATE
        <memory>
        <string>
        <fmt/base.h>
    )
    message(STATUS "Precompiled headers enabled for executables")
endif()
