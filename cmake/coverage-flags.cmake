# Source-based coverage instrumentation flags (clang)
# このファイルは add_subdirectory() より前に include すること。
# add_compile_options / add_link_options はその後に追加されたターゲットに適用される。

option(ENABLE_COVERAGE "Enable source-based coverage instrumentation" OFF)

if(NOT ENABLE_COVERAGE)
    return()
endif()

# clang 系コンパイラが必要（AppleClang も対応）
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    message(WARNING "Coverage: clang required (current: ${CMAKE_CXX_COMPILER_ID}). Coverage disabled.")
    return()
endif()

message(STATUS "Coverage     : enabled (source-based, clang)")

add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
add_link_options(-fprofile-instr-generate)
