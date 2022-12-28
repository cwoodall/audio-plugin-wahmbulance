#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "VariableFreqBiquadFilter.h"
#include "EnvelopeFollower.h"

class WahmbulanceProcessor : public juce::AudioProcessor {
public:
    WahmbulanceProcessor();
    ~WahmbulanceProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

private:
    juce::AudioParameterFloat *filterStartingFreqHz;
    juce::AudioParameterFloat *filterResonance;
    juce::AudioParameterFloat *filterRangeHz;
    juce::AudioParameterChoice *filterType;

    juce::AudioParameterFloat *envelopeSensitivity;
    juce::AudioParameterFloat *envelopeAttackMs;
    juce::AudioParameterFloat *envelopeReleaseMs;

    juce::AudioParameterFloat *outputGain;
    juce::AudioParameterFloat *outputMix;

    float sampleRateHz;
    std::vector<float> cutoff_freqs;
    std::vector<float> qs;
    std::vector<float> envelope_outs;
    std::vector<float> signal_copy;

    std::vector<VariableFreqBiquadFilter> filter;
    std::vector<EnvelopeFollower> envelope_follower;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WahmbulanceProcessor)
    
    friend class WahmbulanceProcessorEditor;
};
