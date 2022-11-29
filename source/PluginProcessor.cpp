#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    addParameter(lpf_cutoff_Hz = new juce::AudioParameterFloat("filter_cutoff", "Filter Cutoff", 0.0f, 10000.0f, 100.0f)); // [2]
    addParameter(q = new juce::AudioParameterFloat("q", "Q", 0.0f, 50.0f, 0.707f)); // [2]
    // addParameter(filter_type = new juce::AudioParameterChoice("filter_type", "Filter Type", juce: )); // [2]
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
    filter.setSamplingRate(static_cast<float>(sampleRate));
    filter.setType(VariableFreqBiquadFilter::Type::LOWPASS);
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    auto gain_copy = gain->get();
    auto lpf_cutoff_copy = lpf_cutoff_Hz->get();
    auto q_copy = q->get();

    filter.setCutoffFrequency(lpf_cutoff_copy);
    filter.setQ(q_copy);

    filter.processBlock(buffer);

    for (auto channel = 0; channel < buffer.getNumChannels(); channel++) {
        // to access the sample in the channel as a C-style array
        auto channelSamples = buffer.getWritePointer(channel);
        for (auto i = 0; i < buffer.getNumSamples(); i++) {
            const auto inputSample = channelSamples[i];
            channelSamples[i] = inputSample * gain_copy;
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
