#pragma once

#include "PluginProcessor.h"
#include "CWLookAndFeel.h"

using namespace std;
using namespace juce;

//==============================================================================
class WahmbulanceProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit WahmbulanceProcessorEditor (WahmbulanceProcessor&);
    ~WahmbulanceProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void paintOverChildren(Graphics&) override;

private:

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WahmbulanceProcessor& processorRef;

    Slider filterStartingFreqHzSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider filterResonanceSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider filterRangeHzSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    // Slider filterTypeSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider envelopeSensitivitySlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider envelopeAttackSSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider envelopeDecaySSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider outputGainSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider outputMixSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };

    SliderParameterAttachment filterStartingFreqHzAttachment;
    SliderParameterAttachment filterResonanceAttachment;
    SliderParameterAttachment filterRangeHzAttachment;
    // SliderParameterAttachment filterTypeAttachment;
    SliderParameterAttachment envelopeSensitivityAttachment;
    SliderParameterAttachment envelopeAttackSAttachment;
    SliderParameterAttachment envelopeDecaySAttachment;
    SliderParameterAttachment outputGainAttachment;
    SliderParameterAttachment outputMixAttachment;

    vector<tuple<Component&, array<float, 4>>> componentPositions;

    unique_ptr<juce::Drawable> knobDrawable;
    unique_ptr<juce::Drawable> knobShadowDrawable;
    unique_ptr<juce::Drawable> knobTopShadowDrawable;
    unique_ptr<juce::Drawable> background;
    unique_ptr<Drawable>background_text;
    unique_ptr<Drawable>overlay;
    unique_ptr<Drawable>debugOverlay;

    CWLookAndFeel lookAndFeel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WahmbulanceProcessorEditor)
};
