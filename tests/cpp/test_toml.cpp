#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <string_view>

#include <doctest/doctest.h>
#include <toml++/toml.hpp>

using namespace std::literals::string_view_literals;

class TomlTestsFixture {
protected:
    static constexpr std::string_view kTomlConfigString = R"(
        [library]
        name = "toml++"
        authors = ["Mark Gillard <mark.gillard@outlook.com.au>"]
        cpp = 17
        pi = 3.14
    )"sv;

    toml::parse_result toml_config_;

public:
    TomlTestsFixture()
        : toml_config_(toml::parse(kTomlConfigString)) {};

    ~TomlTestsFixture() = default;
};

TEST_CASE("testing GeneratorLinear") {
    static constexpr std::string_view kTomlConfigString = R"(
        [library]
        name = "toml++"
        cpp = 17
        pi = 3.14
    )"sv;
    auto toml_config = toml::parse(kTomlConfigString);
    auto pi = toml_config["library"]["pi"].value_or(0.0);

    CHECK(doctest::Approx(3.14).epsilon(0.001) == pi);
}

TEST_CASE_FIXTURE(TomlTestsFixture, "testing GeneratorQuadratic") {
    auto result = toml_config_["library"]["pi"].value_or(3.14);
    CHECK(result == doctest::Approx(3.14).epsilon(0.001));
}
