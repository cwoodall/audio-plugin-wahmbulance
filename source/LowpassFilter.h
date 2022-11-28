#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

class LowpassFilter {
public:
    void setSamplingRate(float samplingRate);
    void setCutoffFrequency(float cutoffFreq);

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &);

private:
    float samplingRate;
    float cutoffFreq;
    std::vector<float> delay_buffer;
};