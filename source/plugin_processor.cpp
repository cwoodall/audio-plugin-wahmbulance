#include "plugin_processor.h"
#include "plugin_editor.h"

#include "dsp_math.h"

using namespace cw::dsp;

//==============================================================================
WahmbulanceProcessor::WahmbulanceProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ) {
    addParameter(filterStartingFreqHz = new juce::AudioParameterFloat("filterStartingFreqHz", "Starting Frequency Hz", juce::NormalisableRange<float>(10.0f, 10000.0f, 1.0f, .3f), 100.0f)); // [2]
    addParameter(filterResonance = new juce::AudioParameterFloat("filterResonance", "Resonance", juce::NormalisableRange<float>(.1f, 20.0f, .1f, 1.0f), 2)); // [2]
    addParameter(filterRangeHz = new juce::AudioParameterFloat("filterRangeHz", "Range", juce::NormalisableRange<float>(-20000.0f, 20000.f, 1.f, .3f, true), 1000.f));
    addParameter(envelopeSensitivity = new juce::AudioParameterFloat("envelopeSensitivity", "Envelope Sensitivity", 0.f, 10.f, 1.f));
    addParameter(envelopeAttackMs = new juce::AudioParameterFloat("envelopeAttackMs", "Envelope Attack", juce::NormalisableRange<float>(.1f, 500.f, .1f, .3f), 5.f));
    addParameter(envelopeReleaseMs = new juce::AudioParameterFloat("envelopeReleaseMs", "Envelope Release", juce::NormalisableRange<float>(.1f, 500.f, .1f, .3f), 100.f));
    addParameter(filterType = new juce::AudioParameterChoice("filterType", "Filter Type", { "Lowpass", "Bandpass", "Highpass" }, 0));
    addParameter(outputMix = new juce::AudioParameterFloat("outputMix", "Mix", 0.f, 1.f, .5f));
    addParameter(outputGain = new juce::AudioParameterFloat("outputGain", "Gain", 0.0f, 1.0f, 0.5f)); // [2]
}

WahmbulanceProcessor::~WahmbulanceProcessor() {
}

//==============================================================================
const juce::String WahmbulanceProcessor::getName() const {
    return JucePlugin_Name;
}

bool WahmbulanceProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool WahmbulanceProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool WahmbulanceProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double WahmbulanceProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int WahmbulanceProcessor::getNumPrograms() {
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int WahmbulanceProcessor::getCurrentProgram() {
    return 0;
}

void WahmbulanceProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String WahmbulanceProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void WahmbulanceProcessor::changeProgramName(int index, const juce::String &newName) {
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void WahmbulanceProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    sampleRateHz = (float) sampleRate;
    cutoff_freqs.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    envelope_outs.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    qs.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    signal_copy.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    filter.resize(static_cast<size_t>(totalNumOutputChannels));
    envelope_follower.resize(static_cast<size_t>(totalNumOutputChannels));
    for (size_t channel = 0; channel < static_cast<size_t>(totalNumOutputChannels); channel++) {
        envelope_follower[channel].setSamplingRate(sampleRateHz);

        filter[channel].setSamplingRate(sampleRateHz);
    }
}

void WahmbulanceProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool WahmbulanceProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void WahmbulanceProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &midiMessages) {
    juce::ignoreUnused(midiMessages);

    size_t total_num_input_channels = (size_t) getTotalNumInputChannels();
    size_t total_num_output_channels = (size_t) getTotalNumOutputChannels();
    size_t num_samples = (size_t) buffer.getNumSamples();
    size_t num_channels = (size_t) buffer.getNumChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (size_t i = total_num_input_channels; i < total_num_output_channels; ++i) {
        buffer.clear((int) i, 0, (int) num_samples);
    }

    float output_gain = outputGain->get();
    float filter_cutoff_hz = filterStartingFreqHz->get();
    float resonance = filterResonance->get();
    AdjustableBiquadFilter::Type filter_type = static_cast<AdjustableBiquadFilter::Type>(filterType->getIndex());
    float filter_range = filterRangeHz->get();
    float envelope_sensitivity = envelopeSensitivity->get();
    float envelope_attack_ms = envelopeAttackMs->get();
    float envelope_release_ms = envelopeReleaseMs->get();
    float mix_copy = outputMix->get();

    std::fill(qs.begin(), qs.end(), resonance);

    for (size_t channel = 0; channel < num_channels; channel++) {
        float *channelSamples = buffer.getWritePointer((int) channel);
        signal_copy.assign(channelSamples, &channelSamples[num_samples]);

        // Run the envelope follower
        envelope_follower[channel].setAttackTimeConstant(envelope_attack_ms);
        envelope_follower[channel].setReleaseTimeConstant(envelope_release_ms);
        envelope_follower[channel].step(num_samples, channelSamples, &envelope_outs[0]);

        // Apply the envelope follower signal to modulate
        for (size_t i = 0; i < num_samples; i++) {
            cutoff_freqs[i] = filter_cutoff_hz
                              + envelope_sensitivity * envelope_outs[i] * filter_range; // Make programmable
            float stop_freq = clip<float>(filter_cutoff_hz + filter_range, 5, sampleRateHz / 2.0f - .1f);
            float min_freq = filter_range < 0 ? stop_freq : 5;
            float max_freq = filter_range >= 0 ? stop_freq : (sampleRateHz / 2.0f - .1f);
            cutoff_freqs[i] = clip<float>(cutoff_freqs[i], min_freq, max_freq);
        }
        filter[channel].setType(filter_type);
        filter[channel].step(num_samples, channelSamples, &cutoff_freqs[0], &qs[0], channelSamples);

        for (size_t i = 0; i < num_samples; i++) {
            channelSamples[i] = (mix_copy * channelSamples[i] + (1 - mix_copy) * signal_copy[i]) * output_gain;
        }

        cutoff_average_freq = reduce(cutoff_freqs.begin(), cutoff_freqs.end()) / cutoff_freqs.size();
    }
}

float WahmbulanceProcessor::getCutoffAverageFreq() { return cutoff_average_freq; }
//==============================================================================
bool WahmbulanceProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *WahmbulanceProcessor::createEditor() {
    return new WahmbulanceProcessorEditor(*this);
}

//==============================================================================
void WahmbulanceProcessor::getStateInformation(juce::MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void WahmbulanceProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new WahmbulanceProcessor();
}
