# CLI11 version command sources
set(CLI11_COMMAND_SOURCES
    cli_cli11.cpp
    subcommand_cli11.cpp
)

# morrisfranken argparse version command sources
set(ARGPARSE_MORRIS_COMMAND_SOURCES
    cli_argparse_morris.cpp
    subcommand_argparse_morris.cpp
)

# Create CLI11 command library
add_library(command_lib STATIC ${CLI11_COMMAND_SOURCES})
target_include_directories(command_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(command_lib PUBLIC CLI11::CLI11 fmt::fmt)

# Create morrisfranken argparse command library
add_library(command_argparse_morris_lib STATIC ${ARGPARSE_MORRIS_COMMAND_SOURCES})
target_include_directories(command_argparse_morris_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(command_argparse_morris_lib PUBLIC argparse_morris_target fmt::fmt)
