# カスタムターゲット定義

# cmake --build build --target test テスト実行
# cmake --build build --target run-tests テスト実行 + 失敗時に詳細出力
add_custom_target(run-tests
    COMMAND ctest --output-on-failure
    COMMENT "Running all tests"
)

# ヘルプターゲット
add_custom_target(show-help
    COMMAND ${CMAKE_COMMAND} -E echo "=== Available Build Targets ==="
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Main applications:"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmd_cli11             - CLI11 based calculator"
    COMMAND ${CMAKE_COMMAND} -E echo "  cmd_argparse_morris   - argparse (morris) based calculator"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Development targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  run-tests             - Run all tests"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Code quality targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  format                - Format code with clang-format"
    COMMAND ${CMAKE_COMMAND} -E echo "  format-dry            - Check formatting without changes"
    COMMAND ${CMAKE_COMMAND} -E echo "  lint                  - Lint code with clang-tidy"
    COMMAND ${CMAKE_COMMAND} -E echo "  run-cppcheck          - Run cppcheck static analysis"
    COMMAND ${CMAKE_COMMAND} -E echo "  run-cppcheck-verbose  - Run cppcheck with verbose output"
    COMMENT "Showing available build targets"
)
