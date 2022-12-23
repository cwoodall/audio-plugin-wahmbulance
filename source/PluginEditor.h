#pragma once

#include "PluginProcessor.h"
#include "CWLookAndFeel.h"

using namespace std;
using namespace juce;
//==============================================================================
class AutoWahProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AutoWahProcessorEditor (AutoWahProcessor&);
    ~AutoWahProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void paintOverChildren(Graphics&) override;

private:

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoWahProcessor& processorRef;
    juce::Slider gainSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label gainLabel;
    juce::SliderParameterAttachment gainAttachment;

    juce::Slider startingFreqSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label startingFreqLabel;
    juce::SliderParameterAttachment lpfAttachment;
    unique_ptr<juce::Drawable> knobDrawable;
    unique_ptr<juce::Drawable> knobShadowDrawable;
    unique_ptr<juce::Drawable> knobTopShadowDrawable;
    unique_ptr<juce::Drawable> background;
    unique_ptr<Drawable>background_text;
    unique_ptr<Drawable>overlay;
    unique_ptr<Drawable>debugOverlay;

    CWLookAndFeel lookAndFeel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoWahProcessorEditor)
};
