#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

//==============================================================================
AutoWahProcessorEditor::AutoWahProcessorEditor(AutoWahProcessor &p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      gainAttachment(*p.gain, gainSlider),
      lpfAttachment(*p.starting_freq_Hz, startingFreqSlider),
      knobDrawable(juce::Drawable::createFromImageData(BinaryData::knob_main_svg, BinaryData::knob_main_svgSize)),
      knobShadowDrawable(juce::Drawable::createFromImageData(BinaryData::knob_shadow_svg, BinaryData::knob_shadow_svgSize)),
      knobTopShadowDrawable(juce::Drawable::createFromImageData(BinaryData::knob_topshadow_svg, BinaryData::knob_topshadow_svgSize)),
      lookAndFeel(*knobDrawable, *knobShadowDrawable, *knobTopShadowDrawable),
      background(juce::Drawable::createFromImageData(BinaryData::background_png, BinaryData::background_pngSize)) {
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(700, 250);

    setLookAndFeel(&lookAndFeel);

    // addAndMakeVisible(gainSlider);
    // gainSlider.setRange(0, 1.0);
    // gainSlider.setTextValueSuffix(" %");
    // addAndMakeVisible(gainLabel);
    // gainLabel.setText("Gain", juce::dontSendNotification);
    // gainLabel.attachToComponent(&gainSlider, true);

    // addAndMakeVisible(startingFreqSlider);
    // startingFreqSlider.setRange(10, 22000.0);
    // startingFreqSlider.setTextValueSuffix(" Hz");
    // addAndMakeVisible(startingFreqLabel);
    // startingFreqLabel.setText("Starting Frequency", juce::dontSendNotification);
    // startingFreqLabel.attachToComponent(&startingFreqSlider, true);
}

AutoWahProcessorEditor::~AutoWahProcessorEditor() {
}

//==============================================================================
void AutoWahProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Draw the background
    auto bounds = getBounds();
    auto centre = bounds.getCentre();
    float scale = bounds.getWidth() / (float) background->getWidth();
    auto scaleTransform = juce::AffineTransform::scale(scale);
    background->setTransform(scaleTransform);
    background->drawAt(g, 0, 0, 1);
}

void AutoWahProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 120;
    gainSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 30, 60);
    startingFreqSlider.setBounds(sliderLeft, 120, getWidth() - sliderLeft - 30, 60);
}