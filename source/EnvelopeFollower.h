#pragma once
#include "VariableFreqBiquadFilter.h"
#include <array>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * @brief 
 * 
 * https://github.com/cwoodall/py-audio-effects-autowah/blob/main/autowah/envelope_follower.py
 */
class EnvelopeFollower {
public:
    EnvelopeFollower();

    void setSamplingRate(float sample_rate);
    void setAttackTimeConstant(float attack_ms);
    void setReleaseTimeConstant(float release_ms);

    void step(size_t n, const float in[], float out[]);

private:
    float envelope_;
    float attack_ms_;
    float release_ms_;
    float  sample_rate_;

    float attack_coef_;
    float release_coef_;
    void calculateGains();
};