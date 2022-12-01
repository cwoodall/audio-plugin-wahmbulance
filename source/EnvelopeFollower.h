#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include "VariableFreqBiquadFilter.h"

/**
 * @brief 
 * 
 * https://github.com/cwoodall/py-audio-effects-autowah/blob/main/autowah/envelope_follower.py
 */
class EnvelopeFollower {
public:
    EnvelopeFollower();

    void setSamplingRate(float sample_rate);
    void setCutoffFrequency(float cutoff_freq);

    void step(size_t n, const float in[], float out[]);
private:
    VariableFreqBiquadFilter lpf;
};
