# FetchContentライブラリの基本的なライセンス収集スクリプト

# FetchContentで取得したパッケージ
set(FETCHCONTENT_PACKAGES "argparse_morris")

foreach(package ${FETCHCONTENT_PACKAGES})
    # パッケージのソースディレクトリを取得
    set(source_dir "${CMAKE_BINARY_DIR}/_deps/${package}-src")

    if(EXISTS "${source_dir}")
        # LICENSEファイルの検索
        file(GLOB license_files "${source_dir}/LICENSE*")
        foreach(license_file ${license_files})
            if(NOT IS_DIRECTORY "${license_file}")
                file(COPY "${license_file}" DESTINATION "${CMAKE_BINARY_DIR}/third_party_licenses/")
                get_filename_component(license_name "${license_file}" NAME)
                file(RENAME
                    "${CMAKE_BINARY_DIR}/third_party_licenses/${license_name}"
                    "${CMAKE_BINARY_DIR}/third_party_licenses/fetchcontent_${package}_${license_name}")
                message(STATUS "Copied license: fetchcontent_${package}_${license_name}")
            endif()
        endforeach()
    else()
        message(WARNING "FetchContent package directory not found: ${source_dir}")
    endif()
endforeach()