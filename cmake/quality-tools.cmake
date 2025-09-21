# Quality management tools configuration
# Usage: include(cmake/quality-tools.cmake)

# Platform-specific quality tools search paths
function(setup_quality_tools)
    # clang-format search
    if(DEFINED CLANG_FORMAT_SEARCH_PATHS)
        string(REPLACE ";" ";" CLANG_FORMAT_PATHS "${CLANG_FORMAT_SEARCH_PATHS}")
        find_program(CLANG_FORMAT_EXE 
            NAMES clang-format
            PATHS ${CLANG_FORMAT_PATHS}
            NO_DEFAULT_PATH
        )
    else()
        # Platform-specific default search
        if(APPLE)
            # macOS: Homebrew paths
            set(DEFAULT_CLANG_FORMAT_PATHS
                "/opt/homebrew/bin"
                "/opt/homebrew/opt/llvm/bin"
                "/opt/homebrew/opt/llvm@*/bin"
            )
        elseif(UNIX)
            # Linux: Package manager and manual install paths
            set(DEFAULT_CLANG_FORMAT_PATHS
                "/usr/bin"
                "/usr/local/bin"
                "/usr/lib/llvm-*/bin"                        # Ubuntu/Debian
                "/opt/rh/llvm-toolset-*/root/usr/bin"        # RHEL SCL
            )
        endif()
        
        if(DEFINED DEFAULT_CLANG_FORMAT_PATHS)
            find_program(CLANG_FORMAT_EXE 
                NAMES clang-format
                PATHS ${DEFAULT_CLANG_FORMAT_PATHS}
                NO_DEFAULT_PATH
            )
        endif()
        
        # Fallback to system PATH
        if(NOT CLANG_FORMAT_EXE)
            find_program(CLANG_FORMAT_EXE NAMES clang-format)
        endif()
    endif()

    # clang-tidy search  
    if(DEFINED CLANG_TIDY_SEARCH_PATHS)
        string(REPLACE ";" ";" CLANG_TIDY_PATHS "${CLANG_TIDY_SEARCH_PATHS}")
        find_program(CLANG_TIDY_EXE 
            NAMES clang-tidy
            PATHS ${CLANG_TIDY_PATHS}
            NO_DEFAULT_PATH
        )
    else()
        # Platform-specific default search
        if(APPLE)
            # macOS: Homebrew LLVM only (not in /opt/homebrew/bin)
            set(DEFAULT_CLANG_TIDY_PATHS
                "/opt/homebrew/opt/llvm/bin"
                "/opt/homebrew/opt/llvm@*/bin"
            )
        elseif(UNIX)
            # Linux: LLVM-specific paths preferred
            set(DEFAULT_CLANG_TIDY_PATHS
                "/usr/lib/llvm-*/bin"                        # Ubuntu/Debian  
                "/opt/rh/llvm-toolset-*/root/usr/bin"        # RHEL SCL
                "/usr/bin"                                   # System fallback
                "/usr/local/bin"                             # Manual install
            )
        endif()
        
        if(DEFINED DEFAULT_CLANG_TIDY_PATHS)
            find_program(CLANG_TIDY_EXE 
                NAMES clang-tidy
                PATHS ${DEFAULT_CLANG_TIDY_PATHS}
                NO_DEFAULT_PATH
            )
        endif()
        
        # Fallback to system PATH
        if(NOT CLANG_TIDY_EXE)
            find_program(CLANG_TIDY_EXE NAMES clang-tidy)
        endif()
    endif()


    # cppcheck search
    find_program(CPPCHECK_EXE NAMES cppcheck)
    
    # Display found tools
    message(STATUS "Quality tools configuration:")
    if(CLANG_FORMAT_EXE)
        message(STATUS  "  clang-format: ${CLANG_FORMAT_EXE}")
    else()
        message(WARNING "  clang-format: NOT FOUND")
    endif()
    
    if(CLANG_TIDY_EXE)
        message(STATUS  "  clang-tidy: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "  clang-tidy: NOT FOUND")
    endif()
    
    
    if(CPPCHECK_EXE)
        message(STATUS  "  cppcheck: ${CPPCHECK_EXE}")
    else()
        message(WARNING "  cppcheck: NOT FOUND")
    endif()
    
    # Set variables for parent scope
    set(CLANG_FORMAT_EXE "${CLANG_FORMAT_EXE}" PARENT_SCOPE)
    set(CLANG_TIDY_EXE "${CLANG_TIDY_EXE}" PARENT_SCOPE)
    set(CPPCHECK_EXE "${CPPCHECK_EXE}" PARENT_SCOPE)
endfunction()

# Setup quality tools targets
function(setup_quality_targets SOURCE_FILES COMPILABLE_SOURCE_FILES)
    if(CLANG_FORMAT_EXE)
        add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE} -i ${SOURCE_FILES}
            COMMENT "Formatting source code with clang-format"
        )
        add_custom_target(format-dry
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${SOURCE_FILES}
            COMMENT "Checking formatting (dry-run)"
        )
    else()
        add_custom_target(format
            COMMAND ${CMAKE_COMMAND} -E echo "clang-format not available"
            COMMENT "clang-format not found - skipping format"
        )
        add_custom_target(format-dry
            COMMAND ${CMAKE_COMMAND} -E echo "clang-format not available"
            COMMENT "clang-format not found - skipping format check"
        )
    endif()

    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" PARENT_SCOPE)

        add_custom_target(lint
            COMMAND ${CLANG_TIDY_EXE}
                -p ${CMAKE_BINARY_DIR}
                --extra-arg=-nostdinc++
                --extra-arg=-isystem/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1
                --extra-arg=-isystem/Library/Developer/CommandLineTools/usr/lib/clang/17/include
                --extra-arg=-isystem/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include
                ${COMPILABLE_SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running clang-tidy"
            VERBATIM
        )
    else()
        add_custom_target(lint
            COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not available"
            COMMENT "clang-tidy not found - skipping lint"
        )
    endif()


    # cppcheck targets
    if(CPPCHECK_EXE)
        # cppcheck arguments
        set(CPPCHECK_BASE_ARGS
            --enable=all
            --inconclusive
            --std=c++17
            --platform=native
            -I include/
            --suppress=missingIncludeSystem
            --suppress=unusedFunction
            --suppress=unusedStructMember
            --inline-suppr
            --quiet
        )
        if(DEFINED CPPCHECK_ADDITIONAL_ARGS)
            string(REPLACE " " ";" CPPCHECK_EXTRA_ARGS "${CPPCHECK_ADDITIONAL_ARGS}")
            list(APPEND CPPCHECK_BASE_ARGS ${CPPCHECK_EXTRA_ARGS})
        endif()
        
        add_custom_target(run-cppcheck
            COMMAND ${CPPCHECK_EXE}
                ${CPPCHECK_BASE_ARGS}
                ${SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running cppcheck static analysis"
            VERBATIM
        )
        
        # cppcheck verbose ver.
        set(CPPCHECK_VERBOSE_ARGS ${CPPCHECK_BASE_ARGS})
        list(REMOVE_ITEM CPPCHECK_VERBOSE_ARGS "--quiet")
        list(APPEND CPPCHECK_VERBOSE_ARGS "--verbose")
        
        add_custom_target(run-cppcheck-verbose
            COMMAND ${CPPCHECK_EXE}
                ${CPPCHECK_VERBOSE_ARGS}
                ${SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running cppcheck static analysis (verbose output)"
            VERBATIM
        )
    else()
        add_custom_target(run-cppcheck
            COMMAND ${CMAKE_COMMAND} -E echo "cppcheck not available"
            COMMENT "cppcheck not found - skipping cppcheck analysis"
        )
        add_custom_target(run-cppcheck-verbose
            COMMAND ${CMAKE_COMMAND} -E echo "cppcheck not available"
            COMMENT "cppcheck not found - skipping verbose cppcheck analysis"
        )
    endif()
endfunction()

# Setup integrated quality check target
function(setup_quality_check_target)
    # Create main check target
    add_custom_target(fullcheck
        COMMENT "Running all quality checks (format, lint, cppcheck)"
    )
    
    # Add available quality targets as dependencies
    if(TARGET format)
        add_dependencies(fullcheck format)
        message(STATUS "  Added 'format' to fullcheck target")
    endif()
    
    if(TARGET lint)
        add_dependencies(fullcheck lint)
        message(STATUS "  Added 'lint' to fullcheck target")
    endif()
    
    if(TARGET run-cppcheck)
        add_dependencies(fullcheck run-cppcheck)
        message(STATUS "  Added 'run-cppcheck' to fullcheck target")
    endif()
    
    
    message(STATUS "Quality fullcheck target 'fullcheck' configured")
    message(STATUS "  Usage: cmake --build build --target fullcheck")
    message(STATUS "  Available targets: format, lint, run-cppcheck, fullcheck")
endfunction()
