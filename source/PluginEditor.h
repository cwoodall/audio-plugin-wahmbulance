#pragma once

#include "PluginProcessor.h"
#include "CWLookAndFeel.h"
//==============================================================================
class AutoWahProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AutoWahProcessorEditor (AutoWahProcessor&);
    ~AutoWahProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    std::unique_ptr<juce::Drawable> svg_drawable_play;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoWahProcessor& processorRef;
    juce::Slider gainSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label gainLabel;
    juce::SliderParameterAttachment gainAttachment;

    juce::Slider startingFreqSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label startingFreqLabel;
    juce::SliderParameterAttachment lpfAttachment;
    std::unique_ptr<juce::Drawable> knobDrawable;
    std::unique_ptr<juce::Drawable> knobShadowDrawable;

    CWLookAndFeel lookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoWahProcessorEditor)
};
