#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <array>

/**
 * @brief 
 * 
 * https://github.com/cwoodall/py-audio-effects-autowah/blob/main/autowah/variable_cutoff_biquad_filter.py
 */
class VariableFreqBiquadFilter {
public:
    enum class Type: unsigned {
        LOWPASS = 0,
        BANDPASS,
        HIGHPASS
    };

    void setSamplingRate(float sample_rate);
    void setCutoffFrequency(float cutoff_freq);
    void setQ(float q);
    void setType(VariableFreqBiquadFilter::Type t);

    void processBlock(juce::AudioBuffer<float> &);
private:
    float sample_rate;
    float cutoff_freq_Hz;
    Type filter_type;
    float q;

    std::vector<std::array<float, 3>> buffer_z;
    std::vector<std::array<float, 2>> buffer_y;

    std::array<float, 3> gains_a;
    std::array<float, 3> gains_b;

    void calculateGains();
};
