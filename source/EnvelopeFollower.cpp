#include "EnvelopeFollower.h"
#include "CWDspMath.h"
#define _USE_MATH_DEFINES
#include <math.h>

EnvelopeFollower::EnvelopeFollower() {
    envelope = 0;
}

void EnvelopeFollower::setSamplingRate(float sampleRate) {
    this->sampleRate = sampleRate;
}

void EnvelopeFollower::setAttackTimeConstant(float attackMs) {
    this->attackMs = clip<float>(attackMs, .1f, 1000.0f);
}

void EnvelopeFollower::setReleaseTimeConstant(float releaseMs) {
    this->releaseMs = clip<float>(releaseMs, .1f, 1000.0f);
}

void EnvelopeFollower::calculateGains() {
    attackCoef = 1.0f - std::expf(-1.0f / (attackMs * sampleRate * 0.001f));
    releaseCoef = 1.0f - std::expf(-1.0f / (releaseMs * sampleRate * 0.001f));
}

void EnvelopeFollower::step(size_t n, const float in[], float out[]) {
    calculateGains();
    for (size_t i = 0; i < n; i++) {
        const float absIn = std::fabs(in[i]);
        envelope += (absIn - envelope) * (absIn > envelope ? attackCoef : releaseCoef);
    }
}
