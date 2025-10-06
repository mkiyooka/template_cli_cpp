#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <vector>

#include <doctest/doctest.h>

#include "sut_example/system_under_test.hpp"

// double型比較のテストサンプル
TEST_CASE("Double comparison tests") {
    SUBCASE("DoubleValue function") {
        CHECK(sut_example::DoubleValue(2.5) == doctest::Approx(5.0));
        CHECK(sut_example::DoubleValue(0.0) == doctest::Approx(0.0));
        CHECK(sut_example::DoubleValue(-1.5) == doctest::Approx(-3.0));
    }
}

// vector型比較のテストサンプル
TEST_CASE("Vector comparison tests") {
    SUBCASE("CreateSequence function") {
        auto result = sut_example::CreateSequence(1, 5);
        std::vector<int> expected = {1, 2, 3, 4, 5};
        CHECK(result == expected);

        auto empty_result = sut_example::CreateSequence(0, 0);
        std::vector<int> empty_expected = {};
        CHECK(empty_result == empty_expected);

        auto negative_start = sut_example::CreateSequence(-2, 3);
        std::vector<int> negative_expected = {-2, -1, 0};
        CHECK(negative_start == negative_expected);
    }
}

// 簡単なクラスのテスト
TEST_CASE("ModuloCalculator class") {
    SUBCASE("modulo operations") {
        const sut_example::ModuloCalculator calc(7);
        CHECK(calc.GetRemainder(15) == 1);
        CHECK(calc.GetRemainder(14) == 0);
        CHECK(calc.GetRemainder(6) == 6);
    }
}
