#include "EnvelopeFollower.h"
#define _USE_MATH_DEFINES
#include <math.h>

EnvelopeFollower::EnvelopeFollower()
{
    lpf.setQ(.707f);
    lpf.setType(VariableFreqBiquadFilter::Type::LOWPASS);
}

void EnvelopeFollower::setSamplingRate(float sample_rate) {
    lpf.setSamplingRate(sample_rate);
}
 
void EnvelopeFollower::setCutoffFrequency(float cutoff_freq) {
    lpf.setCutoffFrequency(cutoff_freq);
}

void EnvelopeFollower::step(size_t n, const float in[], float out[]) {
    lpf.calculateGains();
    for (size_t i = 0; i < n; i++) {
        const float abs_in = std::fabs(in[i]);
        lpf.singleStep(abs_in, &out[i]);
    }
}
