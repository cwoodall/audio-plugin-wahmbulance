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
      background(juce::Drawable::createFromImageData(BinaryData::background_png, BinaryData::background_pngSize)),
      background_text(juce::Drawable::createFromImageData(BinaryData::background_text_png, BinaryData::background_text_pngSize)),
      overlay(juce::Drawable::createFromImageData(BinaryData::shadow_overlay_png, BinaryData::shadow_overlay_pngSize)),
      debugOverlay(juce::Drawable::createFromImageData(BinaryData::debug_overlay_png, BinaryData::debug_overlay_pngSize)),
      lookAndFeel(*knobDrawable, *knobShadowDrawable, *knobTopShadowDrawable) {
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(700, 250);

    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(gainSlider);
    gainSlider.setRange(0, 1.0);
    gainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    // addAndMakeVisible(gainLabel);
    // gainLabel.setText("Gain", juce::dontSendNotification);
    // gainLabel.attachToComponent(&gainSlider, true);

    addAndMakeVisible(startingFreqSlider);
    startingFreqSlider.setRange(10, 22000.0);
    startingFreqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
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
    float scale = bounds.getWidth() / (float) background->getWidth();
    auto scaleTransform = juce::AffineTransform::scale(scale);
    background->setTransform(scaleTransform);
    background->drawAt(g, 0, 0, 1);

    // Draw the background
    scale = bounds.getWidth() / (float) background_text->getWidth();
    scaleTransform = juce::AffineTransform::scale(scale);
    background_text->setTransform(scaleTransform);
    background_text->drawAt(g, 0, 0, 1);
}

void AutoWahProcessorEditor::paintOverChildren(juce::Graphics &g) {
    auto bounds = getBounds();
    float scale = bounds.getWidth() / (float) overlay->getWidth();
    auto scaleTransform = juce::AffineTransform::scale(scale);
    overlay->setTransform(scaleTransform);
    overlay->drawAt(g, 0, 0, 1);

    // scale = bounds.getWidth() / (float) debugOverlay->getWidth();
    // scaleTransform = juce::AffineTransform::scale(scale);
    // debugOverlay->setTransform(scaleTransform);
    // debugOverlay->drawAt(g, 0, 0, 1);
}


void AutoWahProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    gainSlider.setBounds(328, 116, 53, 53);
    startingFreqSlider.setBounds(30, 17, 53, 53);
}