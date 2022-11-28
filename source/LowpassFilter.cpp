#include "LowpassFilter.h"

void LowpassFilter::setSamplingRate(float newSamplingRate) {
    this->samplingRate = newSamplingRate;
}
void LowpassFilter::setCutoffFrequency(float newcutoffFreq) {
    this->cutoffFreq = newcutoffFreq;
}

void LowpassFilter::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &) {
    constexpr auto PI = 3.14159265359f;

    delay_buffer.resize(static_cast<size_t>(buffer.getNumChannels()), 0.0f);

    const auto tan = std::tan(PI * cutoffFreq / samplingRate);
    const auto a1 = (tan - 1.f) / (tan + 1.f);

    for (auto channel = 0; channel < buffer.getNumChannels(); channel++) {
        // to access the sample in the channel as a C-style array
        auto channelSamples = buffer.getWritePointer(channel);
        for (auto i = 0; i < buffer.getNumSamples(); i++) {
            const auto inputSample = channelSamples[i];

            const auto allpassFilteredSample = a1 * inputSample + delay_buffer[static_cast<size_t>(channel)];
            delay_buffer[static_cast<size_t>(channel)] = inputSample - a1 * allpassFilteredSample;

            const auto filterOutput = 0.5f * (inputSample + allpassFilteredSample);

            channelSamples[i] = filterOutput;
        }
    }
}
