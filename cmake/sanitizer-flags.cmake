# AddressSanitizer + UndefinedBehaviorSanitizer instrumentation flags
# このファイルは add_subdirectory() より前に include すること。
# add_compile_options / add_link_options はその後に追加されたターゲットに適用される。

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

if(NOT ENABLE_SANITIZERS)
    return()
endif()

# clang / AppleClang / GCC すべて対応
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    message(WARNING "Sanitizers: clang or gcc required (current: ${CMAKE_CXX_COMPILER_ID}). Disabled.")
    return()
endif()

message(STATUS "Sanitizers   : ASan + UBSan enabled (${CMAKE_CXX_COMPILER_ID})")

add_compile_options(
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
    -g
    -O1
)
add_link_options(-fsanitize=address,undefined)
