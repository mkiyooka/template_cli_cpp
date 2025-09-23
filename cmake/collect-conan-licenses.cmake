# Conanライブラリの基本的なライセンス収集スクリプト

# Conanパッケージディレクトリの検索
file(GLOB CONAN_DIRS "${CMAKE_BINARY_DIR}/*/")

foreach(conan_dir ${CONAN_DIRS})
    # Conanパッケージかチェック
    if(EXISTS "${conan_dir}/conaninfo.txt")
        get_filename_component(package_name "${conan_dir}" NAME)

        # LICENSEファイルの検索
        file(GLOB license_files "${conan_dir}/LICENSE*")
        foreach(license_file ${license_files})
            if(NOT IS_DIRECTORY "${license_file}")
                file(COPY "${license_file}" DESTINATION "${CMAKE_BINARY_DIR}/third_party_licenses/")
                get_filename_component(license_name "${license_file}" NAME)
                file(RENAME
                    "${CMAKE_BINARY_DIR}/third_party_licenses/${license_name}"
                    "${CMAKE_BINARY_DIR}/third_party_licenses/${package_name}_${license_name}")
                message(STATUS "Copied license: ${package_name}_${license_name}")
            endif()
        endforeach()
    endif()
endforeach()