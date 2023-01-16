#pragma once

#include "CWLookAndFeel.h"
#include "PluginProcessor.h"
#include <cmp_plot.h>


using namespace std;
using namespace juce;

enum RadioButtonIds {
    FilterTypeRadioGroupId = 1001
};

//==============================================================================
class WahmbulanceProcessorEditor : public juce::AudioProcessorEditor, private Timer {
public:
    explicit WahmbulanceProcessorEditor(WahmbulanceProcessor &);
    ~WahmbulanceProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

    void paintOverChildren(Graphics &) override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WahmbulanceProcessor &processorRef;

    Slider filterStartingFreqHzSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider filterResonanceSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider filterRangeHzSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider envelopeSensitivitySlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider envelopeAttackMsSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider envelopeReleaseMsSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider outputGainSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    Slider outputMixSlider { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };

    SliderParameterAttachment filterStartingFreqHzAttachment;
    SliderParameterAttachment filterResonanceAttachment;
    SliderParameterAttachment filterRangeHzAttachment;
    SliderParameterAttachment envelopeSensitivityAttachment;
    SliderParameterAttachment envelopeAttackMsAttachment;
    SliderParameterAttachment envelopeReleaseMsAttachment;
    SliderParameterAttachment outputGainAttachment;
    SliderParameterAttachment outputMixAttachment;

    ComboBox filterTypeComboBox;
    ComboBoxParameterAttachment filterTypeAttachment;

    vector<tuple<Component &, array<float, 4>>> componentPositions;

    Label filterStartingFreqHzValueLabel;
    Label filterResonanceValueLabel;
    Label filterRangeHzValueLabel;
    Label envelopeSensitivityValueLabel;
    Label envelopeAttackMsValueLabel;
    Label envelopeReleaseMsValueLabel;
    Label outputGainValueLabel;
    Label outputMixValueLabel;
    Label filterStartingFreqHzLabel;
    Label filterResonanceLabel;
    Label filterRangeHzLabel;
    Label envelopeSensitivityLabel;
    Label envelopeAttackMsLabel;
    Label envelopeReleaseMsLabel;
    Label outputGainLabel;
    Label outputMixLabel;

    unique_ptr<juce::Drawable> knobDrawable;
    unique_ptr<juce::Drawable> knobShadowDrawable;
    unique_ptr<juce::Drawable> knobTopShadowDrawable;
    unique_ptr<juce::Drawable> background;
    unique_ptr<Drawable> background_text;
    unique_ptr<Drawable> overlay;
    unique_ptr<Drawable> debugOverlay;
    Font displayFont;
    Font mainFont;

    CWLookAndFeel lookAndFeel;

    cmp::SemiLogX output_plot;
    std::vector<float> output_plot_freq_values;
    std::vector<std::vector<float>> output_plot_y_values;
    std::vector<std::vector<float>> output_plot_x_values;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WahmbulanceProcessorEditor)
};
