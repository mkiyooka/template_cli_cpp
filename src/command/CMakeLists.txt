# CLI11 version command sources
set(CLI11_COMMAND_SOURCES
    cli_cli11.cpp
    subcommand_cli11.cpp
    ${PROJECT_SOURCE_DIR}/src/sut_example/system_under_test.cpp
)

# Create CLI11 command library
add_library(command_lib STATIC ${CLI11_COMMAND_SOURCES})
target_include_directories(command_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(command_lib PUBLIC CLI11::CLI11 fmt::fmt yyjson spdlog::spdlog)
