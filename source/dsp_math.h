#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <complex>
#include <vector>

namespace cw {
namespace dsp {
    constexpr float PI = static_cast<float>(3.14159265);
    template <typename T>
    constexpr T clip(const T n, const T lower, const T upper) {
        return std::max(lower, std::min(n, upper));
    }

    template <typename T, typename Iter>
    constexpr void clip(Iter begin, Iter end, const T lower, const T upper) {
        while (begin != end) {
            *begin = std::max(lower, std::min(*begin, upper));
            begin++;
        }
    }

    /* Get vector of sine wave. */
    template <class T>
    std::vector<T> logspace(const std::size_t length,
                            T min,
                            T max) {
        std::vector<T> retval(length);

        auto start = std::log10(min);
        auto stop = std::log10(max);
        auto dx = (stop - start) / length;

        auto last_x = min;
        for (auto &x : retval) {
            x = std::pow(10.0f, std::log10(last_x) + dx);
            last_x = x;
        }
        return retval;
    }

} // namespace dsp
} // namespace cw
