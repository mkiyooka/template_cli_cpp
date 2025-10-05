# テストフレームワーク用のサードパーティライブラリ定義

# doctest - Testing framework
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
declare_fetchcontent_with_local(doctest ext/doctest-2.4.11
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

# Mark doctest as system library to exclude it from clang-tidy checks
if(TARGET doctest)
    get_target_property(doctest_include_dirs doctest INTERFACE_INCLUDE_DIRECTORIES)
    if(doctest_include_dirs)
        set_target_properties(doctest PROPERTIES
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${doctest_include_dirs}"
        )
    endif()
endif()
