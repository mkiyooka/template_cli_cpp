# ベンチマークバイナリは build/benches/ 以下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benches)

# Logger benchmark
add_executable(bench_logger
    bench_logger.cpp
)
target_include_directories(bench_logger PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(bench_logger PRIVATE
    spdlog::spdlog
    nanobench::nanobench
)
