# Quality management tools configuration
# Usage: include(cmake/quality-tools.cmake)

# Tool runner configuration (pixi support)
function(setup_tool_runner)
    # Check if pixi is available and project has pixi.toml
    find_program(PIXI_EXE NAMES pixi)
    if(PIXI_EXE AND EXISTS "${CMAKE_SOURCE_DIR}/pixi.toml")
        set(TOOL_RUNNER "${PIXI_EXE}" "run" PARENT_SCOPE)
        message(STATUS "Using pixi for tool execution: ${PIXI_EXE}")
    else()
        set(TOOL_RUNNER "" PARENT_SCOPE)
        message(STATUS "Using system tools directly (pixi not available or no pixi.toml)")
    endif()
endfunction()

# Platform-specific quality tools search paths
function(setup_quality_tools)
    # Setup tool runner (pixi support)
    setup_tool_runner()

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
    set(TOOL_RUNNER "${TOOL_RUNNER}" PARENT_SCOPE)
endfunction()

# Setup quality tools targets
function(setup_quality_targets SOURCE_FILES COMPILABLE_SOURCE_FILES)
    if(CLANG_FORMAT_EXE)
        add_custom_target(format
            COMMAND ${TOOL_RUNNER} clang-format -i ${SOURCE_FILES}
            COMMENT "Formatting source code with clang-format"
        )
        add_custom_target(format-dry
            COMMAND ${TOOL_RUNNER} clang-format --dry-run --Werror ${SOURCE_FILES}
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

        # Check for run-clang-tidy for parallel execution
        find_program(RUN_CLANG_TIDY_EXE NAMES run-clang-tidy)

        if(RUN_CLANG_TIDY_EXE)
            # Parallel execution with run-clang-tidy
            include(ProcessorCount)
            ProcessorCount(N)
            if(NOT N EQUAL 0)
                math(EXPR LINT_JOBS "${N} / 2")  # Use half of available cores
                if(LINT_JOBS LESS 1)
                    set(LINT_JOBS 1)
                endif()
            else()
                set(LINT_JOBS 4)
            endif()

            add_custom_target(lint
                COMMAND ${TOOL_RUNNER} run-clang-tidy
                    -p ${CMAKE_BINARY_DIR}
                    -quiet
                    -j ${LINT_JOBS}
                    ${COMPILABLE_SOURCE_FILES}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running clang-tidy (${LINT_JOBS} parallel jobs)"
                VERBATIM
            )
        else()
            # Sequential execution with clang-tidy
            add_custom_target(lint
                COMMAND ${TOOL_RUNNER} clang-tidy
                    -p ${CMAKE_BINARY_DIR}
                    --quiet
                    ${COMPILABLE_SOURCE_FILES}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running clang-tidy"
                VERBATIM
            )
        endif()
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
            --enable=warning,style,performance,portability
            --std=c++17
            --platform=native
            -I include/
            --suppress=missingIncludeSystem
            --suppress=unusedFunction
            --suppress=unusedStructMember
            --suppress=ctuOneDefinitionRuleViolation
            --suppress=normalCheckLevelMaxBranches
            --suppress=unmatchedSuppression
            --inline-suppr
            --quiet
        )
        if(DEFINED CPPCHECK_ADDITIONAL_ARGS)
            string(REPLACE " " ";" CPPCHECK_EXTRA_ARGS "${CPPCHECK_ADDITIONAL_ARGS}")
            list(APPEND CPPCHECK_BASE_ARGS ${CPPCHECK_EXTRA_ARGS})
        endif()
        
        add_custom_target(run-cppcheck
            COMMAND ${TOOL_RUNNER} cppcheck
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
            COMMAND ${TOOL_RUNNER} cppcheck
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
