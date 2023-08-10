#include "EnvelopeFollower.h"
#include "CWDspMath.h"
#define _USE_MATH_DEFINES
#include <math.h>

EnvelopeFollower::EnvelopeFollower() {
    envelope_ = 0;
}

void EnvelopeFollower::setSamplingRate(float sample_rate) {
    this->sample_rate_ = sample_rate;
}

void EnvelopeFollower::setAttackTimeConstant(float attack_ms) {
    this->attack_ms_ = clip<float>(attack_ms, .1f, 1000.0f);
}

void EnvelopeFollower::setReleaseTimeConstant(float release_ms) {
    this->release_ms_ = clip<float>(release_ms, .1f, 1000.0f);
}

void EnvelopeFollower::calculateGains() {
    this->attack_coef_ = 1.0f - std::exp(-1.0f / (attack_ms_ * sample_rate_ * 0.001f));
    this->release_coef_ = 1.0f - std::exp(-1.0f / (release_ms_ * sample_rate_ * 0.001f));
}

void EnvelopeFollower::step(size_t n, const float in[], float out[]) {
    calculateGains();
    for (size_t i = 0; i < n; i++) {
        const float abs_in = std::fabs(in[i]);
        envelope_ += (abs_in - envelope_) * (abs_in > envelope_ ? attack_coef_ : release_coef_);
        out[i] = envelope_;
    }
}
