#include "adjustable_biquad_filter.h"
#include "dsp_math.h"

#define _USE_MATH_DEFINES
#include <cmath>

AdjustableBiquadFilter::AdjustableBiquadFilter() {
    first_run = true;
}

void AdjustableBiquadFilter::setSamplingRate(float sr) {
    this->sample_rate = sr;
}

void AdjustableBiquadFilter::setCutoffFrequency(float cf) {
    this->cutoff_freq_Hz = cf;
}

void AdjustableBiquadFilter::setQ(float q_) {
    this->q = q_;
}

void AdjustableBiquadFilter::setType(AdjustableBiquadFilter::Type t) {
    this->filter_type = t;
}

void AdjustableBiquadFilter::step(const size_t n, const float in[], float out[]) {
    calculateGains();
    for (size_t i = 0; i < n; i++) {
        singleStep(in[i], &out[i]);
    }
}
void AdjustableBiquadFilter::step(const size_t n,
                                  const float in[],
                                  const float cutoff_freqs[],
                                  const float qs[],
                                  float out[]) {
    for (size_t i = 0; i < n; i++) {
        setCutoffFrequency(cutoff_freqs[i]);
        setQ(qs[i]);
        calculateGains();

        singleStep(in[i], &out[i]);
    }
}

void AdjustableBiquadFilter::singleStep(const float in, float *out) {
    if (first_run) {
        buffer_z[2] = in;
        buffer_z[1] = in;
        buffer_z[0] = in;
        buffer_y[1] = in * (1 - gains_b[2] * gains_b[1] * gains_b[0]) / gains_a[1];
        buffer_y[2] = 0;
        first_run = false;
    } else {
        buffer_z[2] = buffer_z[1];
        buffer_z[1] = buffer_z[0];
        buffer_z[0] = in;

        *out = gains_b[2] * buffer_z[2]
               + gains_b[1] * buffer_z[1]
               + gains_b[0] * buffer_z[0]
               - gains_a[1] * buffer_y[0]
               - gains_a[2] * buffer_y[1];

        buffer_y[1] = buffer_y[0];
        buffer_y[0] = *out;
    }
}

void AdjustableBiquadFilter::calculateGains() {
    float wc = cutoff_freq_Hz / sample_rate;
    float k = std::tan(static_cast<float>(cw::dsp::PI) * wc);
    float norm = 1.f / (1.f + k / this->q + k * k);

    switch (filter_type) {
        case AdjustableBiquadFilter::Type::LOWPASS: {
            gains_b[0] = k * k * norm;
            gains_b[1] = 2.f * gains_b[0];
            gains_b[2] = gains_b[0];
            gains_a[0] = 0.0f;
            gains_a[1] = 2.f * (k * k - 1) * norm;
            gains_a[2] = (1.f - k / q + k * k) * norm;
            break;
        }
        case AdjustableBiquadFilter::Type::BANDPASS: {
            gains_b[0] = k / q * norm;
            gains_b[1] = 0.0f;
            gains_b[2] = -gains_b[0];
            gains_a[0] = 0.0f;
            gains_a[1] = 2.0f * (k * k - 1.0f) * norm;
            gains_a[2] = (1.0f - k / q + k * k) * norm;

            break;
        }
        case AdjustableBiquadFilter::Type::HIGHPASS:
        default: {
            gains_b[0] = norm;
            gains_b[1] = -2.0f * gains_b[0];
            gains_b[2] = gains_b[0];
            gains_a[0] = 0;
            gains_a[1] = 2 * (k * k - 1) * norm;
            gains_a[2] = (1 - k / q + k * k) * norm;
            break;
        }
    }
}
