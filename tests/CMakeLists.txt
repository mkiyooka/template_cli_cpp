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
