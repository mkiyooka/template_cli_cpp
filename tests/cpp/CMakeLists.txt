# Test executable
add_executable(test_toml
    test_toml.cpp
)

target_include_directories(test_toml PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(test_toml PRIVATE config_lib doctest::doctest)

# Register the test with CTest
add_test(
    NAME test_toml
    COMMAND $<TARGET_FILE:test_toml>
)
