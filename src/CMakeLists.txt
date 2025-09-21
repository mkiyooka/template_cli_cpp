# Add subdirectories
add_subdirectory(command)
add_subdirectory(config)

# Main executable
add_executable(cmd main.cpp)
target_link_libraries(cmd PRIVATE command_lib config_lib)
target_link_libraries(cmd PUBLIC fmt::fmt)

# プリコンパイル済みヘッダーの設定
if(USE_PCH)
    target_precompile_headers(cmd PRIVATE
        <memory>
        <string>
        <CLI/CLI.hpp>
        <fmt/base.h>
    )
    message(STATUS "Precompiled headers enabled for main executable")
endif()
