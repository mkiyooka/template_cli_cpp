# ローカルディレクトリが存在する場合はそれを使用し、
# 存在しない場合はGitHubから取得する関数
#
# 使用例:
#   declare_fetchcontent_with_local(CLI11 ext/CLI11-2.5.0
#       GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
#       GIT_TAG v2.5.0
#   )
#   FetchContent_MakeAvailable(CLI11)
#
# 引数:
#   LIBRARY_NAME: ライブラリ名（FetchContent_Declareで使用する名前）
#   LOCAL_PATH: ローカルディレクトリパス（プロジェクトルートからの相対パス）
#   GIT_REPOSITORY: GitリポジトリURL
#   GIT_TAG: Gitタグまたはブランチ名
#
# 注意:
#   FetchContent_MakeAvailable()は呼び出し側で実行してください。
#   これにより、MakeAvailable前にライブラリ固有のオプション設定が可能です。
#   例：libzmqの場合、BUILD_TESTS OFF などの設定が必要

function(declare_fetchcontent_with_local LIBRARY_NAME LOCAL_PATH)
    set(options "")
    set(oneValueArgs GIT_REPOSITORY GIT_TAG)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(FULL_LOCAL_PATH "${CMAKE_SOURCE_DIR}/${LOCAL_PATH}")
    if(EXISTS ${FULL_LOCAL_PATH})
        message(STATUS "Using local ${LIBRARY_NAME} from ${FULL_LOCAL_PATH}")
        FetchContent_Declare(${LIBRARY_NAME}
            SOURCE_DIR ${FULL_LOCAL_PATH}
        )
    else()
        message(STATUS "Fetching ${LIBRARY_NAME} from ${ARG_GIT_REPOSITORY}")
        FetchContent_Declare(${LIBRARY_NAME}
            GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
            GIT_TAG ${ARG_GIT_TAG}
        )
    endif()
endfunction()
