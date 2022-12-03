#include "PluginProcessor.h"
#include "PluginEditor.h"

template <typename T>
T clip(const T &n, const T &lower, const T &upper) {
    return std::max(lower, std::min(n, upper));
}

//==============================================================================
AutoWahProcessor::AutoWahProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ) {
    addParameter(gain = new juce::AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.5f)); // [2]
    addParameter(starting_freq_Hz = new juce::AudioParameterFloat("starting_freq_Hz", "Starting Frequency Hz", 10.0f, 10000.0f, 100.0f)); // [2]
    addParameter(q = new juce::AudioParameterFloat("q", "Q", 0.1f, 20.0f, 0.707f)); // [2]
    addParameter(sensitivity = new juce::AudioParameterFloat("sensitivity", "Sensitivity", 0.f, 20000.f, 10000.f));
    addParameter(envelope_gain = new juce::AudioParameterFloat("envelope_gain", "Envelope Gain", 0.f, 10.f, 1.f));
    addParameter(envelope_lpf_Hz = new juce::AudioParameterFloat("envelope_lpf_Hz", "Envelope LPF Hz", .1f, 20000.f, 1.f));
    addParameter(filter_type = new juce::AudioParameterChoice("filter_type", "Filter Type", { "Lowpass", "Bandpass", "Highpass" }, 0));
    addParameter(mix = new juce::AudioParameterFloat("mix", "Mix", 0.f, 1.f, .5f));
}

AutoWahProcessor::~AutoWahProcessor() {
}

//==============================================================================
const juce::String AutoWahProcessor::getName() const {
    return JucePlugin_Name;
}

bool AutoWahProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AutoWahProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AutoWahProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AutoWahProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int AutoWahProcessor::getNumPrograms() {
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int AutoWahProcessor::getCurrentProgram() {
    return 0;
}

void AutoWahProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String AutoWahProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void AutoWahProcessor::changeProgramName(int index, const juce::String &newName) {
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AutoWahProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    sample_rate = (float) sampleRate;
    cutoff_freqs.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    envelope_outs.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    qs.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    signal_copy.resize(static_cast<size_t>(samplesPerBlock), 0.f);
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    filter.resize(static_cast<size_t>(totalNumOutputChannels));
    envelope_follower.resize(static_cast<size_t>(totalNumOutputChannels));
    for (size_t channel = 0; channel < static_cast<size_t>(totalNumOutputChannels); channel++) {
        envelope_follower[channel].setSamplingRate(sample_rate);

        filter[channel].setSamplingRate(sample_rate);
    }
}

void AutoWahProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AutoWahProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
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

void AutoWahProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                    juce::MidiBuffer &midiMessages) {
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    size_t num_samples = static_cast<size_t>(buffer.getNumSamples());
    size_t num_channels = static_cast<size_t>(buffer.getNumChannels());
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, (int) num_samples);
    }

    float gain_copy = gain->get();
    float lpf_cutoff_copy = starting_freq_Hz->get();
    float q_copy = q->get();
    auto filter_type_copy = static_cast<VariableFreqBiquadFilter::Type>(filter_type->getIndex());
    auto sensitivity_copy = sensitivity->get();
    auto envelope_gain_copy = envelope_gain->get();
    auto envelope_lpf_Hz_copy = envelope_lpf_Hz->get();
    auto mix_copy = mix->get();

    // TODO: replace these fills with the envelope follower
    std::fill(qs.begin(), qs.end(), q_copy);

    for (size_t channel = 0; channel < num_channels; channel++) {
        float *channelSamples = buffer.getWritePointer((int) channel);
        signal_copy.assign(channelSamples, channelSamples + num_samples);

        // Run the envelope follower
        envelope_follower[channel].setCutoffFrequency(envelope_lpf_Hz_copy);
        envelope_follower[channel].step(num_samples, channelSamples, &envelope_outs[0]);

        // Apply the envelope follower signal to modulate 
        for (size_t i = 0; i < num_samples; i++) {
            cutoff_freqs[i] = lpf_cutoff_copy
                              + envelope_gain_copy * envelope_outs[i] * sensitivity_copy; // Make programmable
            cutoff_freqs[i] = clip<float>(cutoff_freqs[i], .1f, sample_rate / 2.0f - .1f);
        }
        filter[channel].setType(filter_type_copy);
        filter[channel].step(num_samples, channelSamples, &cutoff_freqs[0], &qs[0], channelSamples);

        for (size_t i = 0; i < num_samples; i++) {
            channelSamples[i] = (mix_copy * channelSamples[i] + (1 - mix_copy) * signal_copy[i]) * gain_copy;
        }
    }
}

//==============================================================================
bool AutoWahProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AutoWahProcessor::createEditor() {
    return new AutoWahProcessorEditor(*this);
}

//==============================================================================
void AutoWahProcessor::getStateInformation(juce::MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void AutoWahProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new AutoWahProcessor();
}
