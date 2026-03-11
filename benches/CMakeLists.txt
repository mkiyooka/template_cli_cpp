# ベンチマークバイナリは build/benches/ 以下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benches)

# Logger benchmark
add_executable(bench_logger
    bench_logger.cpp
)
target_include_directories(bench_logger PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
)
target_link_libraries(bench_logger PRIVATE
    spdlog::spdlog
    nanobench::nanobench
)

# JSON benchmark
add_executable(bench_json
    bench_json.cpp
)
target_include_directories(bench_json PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(bench_json PRIVATE
    yyjson
    nlohmann_json::nlohmann_json
    nanobench::nanobench
)

# CSV benchmark
add_executable(bench_csv
    bench_csv.cpp
)
target_include_directories(bench_csv PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(bench_csv PRIVATE
    csv
    nanobench::nanobench
)
