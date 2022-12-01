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
    VariableFreqBiquadFilter();
    
    void setSamplingRate(float sample_rate);
    void setCutoffFrequency(float cutoff_freq);
    void setQ(float q);
    void setType(VariableFreqBiquadFilter::Type t);
    void calculateGains();

    void step(size_t n, const float in[], const float cutoff_freq[], const float q[], float out[]);
    void singleStep(const float in, float *out);
    void step(const size_t n, const float in[], float out[]);

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
