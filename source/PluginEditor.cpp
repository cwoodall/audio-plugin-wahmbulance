#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

//==============================================================================
AutoWahProcessorEditor::AutoWahProcessorEditor(AutoWahProcessor &p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      gainAttachment(*p.gain, gainSlider),
      lpfAttachment(*p.starting_freq_Hz, startingFreqSlider),
      knobDrawable(juce::Drawable::createFromImageData(BinaryData::knob_svg, BinaryData::knob_svgSize)),
      knobShadowDrawable(juce::Drawable::createFromImageData(BinaryData::knob_shadow_svg, BinaryData::knob_shadow_svgSize)),
      lookAndFeel(*knobDrawable, *knobShadowDrawable) {

    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(gainSlider);
    gainSlider.setRange(0, 1.0); // [1]
    gainSlider.setTextValueSuffix(" %"); // [2]
    addAndMakeVisible(gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, true); // [4]

    addAndMakeVisible(startingFreqSlider);
    startingFreqSlider.setRange(10, 22000.0); // [1]
    startingFreqSlider.setTextValueSuffix(" Hz"); // [2]
    addAndMakeVisible(startingFreqLabel);
    startingFreqLabel.setText("Starting Frequency", juce::dontSendNotification);
    startingFreqLabel.attachToComponent(&startingFreqSlider, true); // [4]
}

AutoWahProcessorEditor::~AutoWahProcessorEditor() {
}

//==============================================================================
void AutoWahProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // svg_drawable_play->setTransformToFit(juce::Rectangle<float>(100, 100, 100, 100), juce::RectanglePlacement::stretchToFit);

    // svg_drawable_play->draw(g, 1.f);
}

void AutoWahProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 120;
    gainSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 100);
    startingFreqSlider.setBounds(sliderLeft, 120, getWidth() - sliderLeft - 10, 100);
}