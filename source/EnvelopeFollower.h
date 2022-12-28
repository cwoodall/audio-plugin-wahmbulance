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

    void setSamplingRate(float sampleRate);
    void setAttackTimeConstant(float attackMs);
    void setReleaseTimeConstant(float releaseMs);

    void step(size_t n, const float in[], float out[]);

private:
    float envelope;
    float attackMs;
    float releaseMs;
    float sampleRate;

    float attackCoef;
    float releaseCoef;
    void calculateGains();
};