#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
AutoWahProcessorEditor::AutoWahProcessorEditor(AutoWahProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), gainAttachment(*p.gain, gainSlider) {
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    addAndMakeVisible(gainSlider);
    gainSlider.setRange(0, 1.0); // [1]
    gainSlider.setTextValueSuffix(" %"); // [2]
    addAndMakeVisible(gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, true); // [4]
}

AutoWahProcessorEditor::~AutoWahProcessorEditor() {
}

//==============================================================================
void AutoWahProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AutoWahProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 120;
    gainSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 100);
}