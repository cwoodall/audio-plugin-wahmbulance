#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <dsp_math.h>

TEST_CASE("clip() passes through when in bounds", "[clip]") {
    CHECK_THAT(
        cw::dsp::clip<float>(0.5f, 0.0f, 1.0f),
        Catch::Matchers::WithinAbs(0.5f, 0.0001f));

    CHECK_THAT(
        cw::dsp::clip<float>(0.0f, 0.0f, 1.0f),
        Catch::Matchers::WithinAbs(0.0f, 0.0001f));

    CHECK_THAT(
        cw::dsp::clip<float>(1.0f, 0.0f, 1.0f),
        Catch::Matchers::WithinAbs(1.0f, 0.0001f));
}

TEST_CASE("clip() constrains through when out of bounds", "[clip]") {
    CHECK_THAT(
        cw::dsp::clip<float>(-1.0f, 0.0f, 1.0f),
        Catch::Matchers::WithinAbs(0.0f, 0.0001f));

    CHECK_THAT(
        cw::dsp::clip<float>(10.0f, 0.0f, 1.0f),
        Catch::Matchers::WithinAbs(1.0f, 0.0001f));
}
