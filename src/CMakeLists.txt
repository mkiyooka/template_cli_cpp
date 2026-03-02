# Add subdirectories
add_subdirectory(command)
add_subdirectory(config)

# CLI11 version - Main executable
add_executable(cmd_cli11 main_cli11.cpp)
set_target_properties(cmd_cli11 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_link_libraries(cmd_cli11 PRIVATE command_lib config_lib)
target_link_libraries(cmd_cli11 PUBLIC fmt::fmt)
set_target_properties(cmd_cli11 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# プリコンパイル済みヘッダーの設定
if(USE_PCH)
    target_precompile_headers(cmd_cli11 PRIVATE
        <memory>
        <string>
        <CLI/CLI.hpp>
        <fmt/base.h>
    )
    message(STATUS "Precompiled headers enabled for executables")
endif()
