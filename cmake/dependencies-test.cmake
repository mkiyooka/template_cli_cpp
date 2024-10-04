# テストフレームワーク用のサードパーティライブラリ定義

# doctest - Testing framework
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
declare_fetchcontent_with_local(doctest third_party/doctest-2.4.11
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)
