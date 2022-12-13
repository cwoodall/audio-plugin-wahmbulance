#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "BinaryData.h"

//==============================================================================
AutoWahProcessorEditor::AutoWahProcessorEditor(AutoWahProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), gainAttachment(*p.gain, gainSlider), 
    lpfAttachment(*p.starting_freq_Hz, starting_freq_slider) {
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);


    auto svg_xml_1(juce::XmlDocument::parse(BinaryData::knob_svg)); // GET THE SVG AS A XML
    // juce::ui::helpers::changeColor(svg_xml_1, "#61f0c4"); // RECOLOUR
    svg_drawable_play = juce::Drawable::createFromSVG(*svg_xml_1); // GET THIS AS DRAWABLE

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

    svg_drawable_play->setTransformToFit(juce::Rectangle<float>(100, 100, 100, 100), juce::RectanglePlacement::stretchToFit);

    svg_drawable_play->draw(g, 1.f);
    
}

void AutoWahProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 120;
    gainSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 100);
    starting_freq_slider.setBounds(sliderLeft, 120, getWidth() - sliderLeft - 10, 100);
}