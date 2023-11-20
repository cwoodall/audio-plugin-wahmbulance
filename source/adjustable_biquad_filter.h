#pragma once

#include "dsp_math.h"
#include <array>
#include <complex>

/**
 * @brief A Variable Frequency Biquad Filter which is capable of being a 2nd order lowpass, bandpass or highpass filter.
 *
 * The original implementation was taken from:
 *
 * - https://github.com/cwoodall/py-audio-effects-autowah/blob/main/autowah/variable_cutoff_biquad_filter.py
 */
class AdjustableBiquadFilter {
public:
    /**
     * Specify the types of biquad filter gains that can be computed.
     */
    enum class Type : unsigned {
        LOWPASS = 0,
        BANDPASS,
        HIGHPASS
    };

    AdjustableBiquadFilter();

    void setSamplingRate(float sample_rate);
    void setCutoffFrequency(float cutoff_freq);
    void setQ(float q);
    void setType(AdjustableBiquadFilter::Type t);
    void calculateGains();

    void step(size_t n, const float in[], const float cutoff_freq[], const float q[], float out[]);
    void step(const size_t n, const float in[], float out[]);
    void singleStep(const float in, float *out);

private:
    float sample_rate;
    float cutoff_freq_Hz;
    Type filter_type;
    float q;

    bool first_run;

    std::array<float, 3> buffer_z;
    std::array<float, 2> buffer_y;

    std::array<float, 3> gains_a;
    std::array<float, 3> gains_b;
};

namespace cw {
namespace dsp {
    template <typename T>
    T second_order_filter_gain(T freq, T fc, T q, AdjustableBiquadFilter::Type type) {
        std::complex<T> sn = std::complex<T>(0.0, 1.0) * freq / fc;
        switch (type) {
            case AdjustableBiquadFilter::Type::LOWPASS: {
                std::complex<T> value = 1.0f / (sn * sn + sn / q + 1.0f);
                return static_cast<T>(std::abs(value));
            }
            case AdjustableBiquadFilter::Type::BANDPASS: {
                std::complex<T> value = (sn / q) / (sn * sn + sn / q + 1.0f);
                return static_cast<T>(std::abs(value));
            }
            case AdjustableBiquadFilter::Type::HIGHPASS: {
                std::complex<T> value = (sn * sn) / (sn * sn + sn / q + 1.0f);
                return static_cast<T>(std::abs(value));
            }
        }
    }

    template <typename T>
    std::vector<T> second_order_filter_response(const std::vector<T> &freqs,
                                                const T fc,
                                                const T resonance,
                                                const AdjustableBiquadFilter::Type type) {
        std::vector<float> retval(freqs.size());

        for (size_t i = 0; i < freqs.size(); i++) {
            retval[i] = second_order_filter_gain(freqs[i], fc, resonance, type);
        }

        return retval;
    }

} // namespace dsp
} // namespace cw
