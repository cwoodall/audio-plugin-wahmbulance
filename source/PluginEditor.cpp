#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
AutoWahProcessorEditor::AutoWahProcessorEditor(AutoWahProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), gainAttachment(*p.gain, gainSlider), 
    lpfAttachment(*p.starting_freq_Hz, starting_freq_slider) {
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

    addAndMakeVisible(starting_freq_slider);
    starting_freq_slider.setRange(10, 22000.0); // [1]
    starting_freq_slider.setTextValueSuffix(" Hz"); // [2]
    addAndMakeVisible(starting_freq_label);
    starting_freq_label.setText("Starting Frequency", juce::dontSendNotification);
    starting_freq_label.attachToComponent(&starting_freq_slider, true); // [4]

}

AutoWahProcessorEditor::~AutoWahProcessorEditor() {
}

//==============================================================================
void AutoWahProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AutoWahProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 120;
    gainSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 100);
    starting_freq_slider.setBounds(sliderLeft, 120, getWidth() - sliderLeft - 10, 100);
}