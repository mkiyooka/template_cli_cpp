# テストバイナリは build/tests/ 以下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# Config manager test
add_executable(test_config_manager
    test_config_manager.cpp
)
target_include_directories(test_config_manager PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/config
    ${PROJECT_SOURCE_DIR}/tests
)
target_link_libraries(test_config_manager PRIVATE config_lib command_lib CLI11::CLI11 doctest::doctest)
add_test(
    NAME test_config_manager
    COMMAND $<TARGET_FILE:test_config_manager>
)

# SUT example library
add_library(sut_example_lib STATIC
    ${PROJECT_SOURCE_DIR}/src/sut_example/system_under_test.cpp
)
target_include_directories(sut_example_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
# No external dependencies needed for simplified implementation

# Test executable
add_executable(test_sut_example
    test_sut_example.cpp
)

target_include_directories(test_sut_example PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(test_sut_example PRIVATE sut_example_lib doctest::doctest)

# Register the test with CTest
add_test(
    NAME test_sut_example
    COMMAND $<TARGET_FILE:test_sut_example>
)

# yyjson_wrapper test
add_executable(test_yyjson_wrapper
    test_yyjson_wrapper.cpp
)
target_include_directories(test_yyjson_wrapper PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(test_yyjson_wrapper PRIVATE
    yyjson
    nlohmann_json::nlohmann_json
    doctest::doctest
)
add_test(
    NAME test_yyjson_wrapper
    COMMAND $<TARGET_FILE:test_yyjson_wrapper>
)
