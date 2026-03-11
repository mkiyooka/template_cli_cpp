# テストフレームワーク用のサードパーティライブラリ定義

# doctest - Testing framework
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
add_external_package(doctest ext/doctest-2.4.12
    URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.12.tar.gz
    URL_HASH SHA256=73381c7aa4dee704bd935609668cf41880ea7f19fa0504a200e13b74999c2d70
)
FetchContent_MakeAvailable(doctest)

# nanobench - Benchmarking library (header-only)
add_external_package(nanobench ext/nanobench-4.3.11
    URL https://github.com/martinus/nanobench/archive/refs/tags/v4.3.11.tar.gz
    URL_HASH SHA256=53a5a913fa695c23546661bf2cd22b299e10a3e994d9ed97daf89b5cada0da70
)
FetchContent_MakeAvailable(nanobench)

# csv-parser - CSV reading library
add_external_package(csv_parser ext/csv-parser-2.5.1
    URL https://github.com/vincentlaucsb/csv-parser/archive/refs/tags/2.5.1.tar.gz
    URL_HASH SHA256=da7128d4946872836f637d5e627cc555cd378e502d866a114a462241ee607da1
)
FetchContent_MakeAvailable(csv_parser)

# Mark doctest as system library to exclude it from clang-tidy checks
if(TARGET doctest)
    get_target_property(doctest_include_dirs doctest INTERFACE_INCLUDE_DIRECTORIES)
    if(doctest_include_dirs)
        set_target_properties(doctest PROPERTIES
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${doctest_include_dirs}"
        )
    endif()
endif()
