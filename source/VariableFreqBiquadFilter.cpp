#include "VariableFreqBiquadFilter.h"
#include <cmath>

void VariableFreqBiquadFilter::setSamplingRate(float sample_rate) {
    this->sample_rate = sample_rate;
}

void VariableFreqBiquadFilter::setCutoffFrequency(float cutoff_freq) {
    this->cutoff_freq_Hz = cutoff_freq;
}

void VariableFreqBiquadFilter::setQ(float q) {
    this->q = q;
}

void VariableFreqBiquadFilter::setType(VariableFreqBiquadFilter::Type t) {
    this->filter_type = t;
}

void VariableFreqBiquadFilter::step(const size_t n, const float in[], const float cutoff_freq[], const float q[], float out[]) {
    for (size_t i = 0; i < n; i++) {
        setCutoffFrequency(cutoff_freq[i]);
        setQ(q[i]);
        calculateGains();

        buffer_z[2] = buffer_z[1];
        buffer_z[1] = buffer_z[0];
        buffer_z[0] = in[i];

        out[i] = gains_b[2] * buffer_z[2]
                 + gains_b[1] * buffer_z[1]
                 + gains_b[0] * buffer_z[0]
                 - gains_a[1] * buffer_y[0]
                 - gains_a[2] * buffer_y[1];
        buffer_y[1] = buffer_y[0];
        buffer_y[0] = out[i];
    }
}

void VariableFreqBiquadFilter::calculateGains() {
    float wc = cutoff_freq_Hz / sample_rate;
    float k = std::tanf(static_cast<float>(M_PI) * wc);
    float norm = 1.f / (1.f + k / this->q + k * k);

    switch (filter_type) {
        case VariableFreqBiquadFilter::Type::LOWPASS: {
            gains_b[0] = k * k * norm;
            gains_b[1] = 2.f * gains_b[0];
            gains_b[2] = gains_b[0];
            gains_a[0] = 0.0f;
            gains_a[1] = 2.f * (k * k - 1) * norm;
            gains_a[2] = (1.f - k / q + k * k) * norm;
            break;
        }
        case VariableFreqBiquadFilter::Type::BANDPASS: {
            gains_b[0] = k / q * norm;
            gains_b[1] = 0.0f;
            gains_b[2] = -gains_b[0];
            gains_a[0] = 0.0f;
            gains_a[1] = 2.0f * (k * k - 1.0f) * norm;
            gains_a[2] = (1.0f - k / q + k * k) * norm;

            break;
        }
        case VariableFreqBiquadFilter::Type::HIGHPASS:
        default: {
            gains_b[0] = 0.f;
            gains_b[1] = 0.f;
            gains_b[2] = 0.f;
            gains_a[0] = 0.f;
            gains_a[1] = 0.f;
            gains_a[2] = 0.f;
            break;
        }
    }
}
