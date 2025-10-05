# コード品質ツールのセットアップ

# Collect source files for quality tools
file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.c
    ${CMAKE_SOURCE_DIR}/tests/*.cpp
    ${CMAKE_SOURCE_DIR}/tests/**/*.cpp
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/include/**/*.hpp
    ${CMAKE_SOURCE_DIR}/include/**/*.h
)

# Separate source files for clang-tidy (only compilable sources)
file(GLOB_RECURSE COMPILABLE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.c
    ${CMAKE_SOURCE_DIR}/tests/*.cpp
    ${CMAKE_SOURCE_DIR}/tests/**/*.cpp
)

# Remove third-party directory files from quality tools processing
file(GLOB_RECURSE THIRD_PARTY_FILES ${CMAKE_SOURCE_DIR}/third_party/**)
file(GLOB_RECURSE EXTERNAL_FILES ${CMAKE_SOURCE_DIR}/external/**)

list(REMOVE_ITEM ALL_SOURCE_FILES ${THIRD_PARTY_FILES} ${EXTERNAL_FILES})
list(REMOVE_ITEM COMPILABLE_SOURCE_FILES ${THIRD_PARTY_FILES} ${EXTERNAL_FILES})

# Setup quality management tools
include(cmake/quality-tools.cmake)
setup_quality_tools()
setup_quality_targets("${ALL_SOURCE_FILES}" "${COMPILABLE_SOURCE_FILES}")
