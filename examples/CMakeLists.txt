# サンプルバイナリは build/examples/ 以下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)

# JSON wrapper example
add_executable(example_json_wrapper
    example_json_wrapper.cpp
)
target_link_libraries(example_json_wrapper PRIVATE
    yyjson
    PRIVATE sanitizer_flags
)
