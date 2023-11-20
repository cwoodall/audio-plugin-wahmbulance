#pragma once

#include "knob_view.h"
#include "plugin_processor.h"
#include "wahm_look_and_feel.h"
#include <cmp_plot.h>

using namespace std;
using namespace juce;

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
    static constexpr uint64_t UPDATE_RATE_HZ = 30;
    static constexpr uint64_t PLUGIN_ORIGINAL_WIDTH_PX = 700;
    static constexpr uint64_t PLUGIN_ORIGINAL_HEIGHT_PX = 250;
    static constexpr uint64_t PLUGIN_ORIGINAL_ROW_0_PX = 17;
    static constexpr uint64_t PLUGIN_ORIGINAL_ROW_1_PX = 116;
    static constexpr uint64_t PLUGIN_ORIGINAL_COLUMN_0_PX = 30;
    static constexpr uint64_t PLUGIN_ORIGINAL_COLUMN_1_PX = 100;
    static constexpr uint64_t PLUGIN_ORIGINAL_COLUMN_2_PX = 170;
    static constexpr uint64_t PLUGIN_ORIGINAL_COLUMN_3_PX = 258;
    static constexpr uint64_t PLUGIN_ORIGINAL_COLUMN_4_PX = 328;
    static constexpr uint64_t PLUGIN_ORIGINAL_KNOB_HEIGHT = 53;
    static constexpr uint64_t PLUGIN_ORIGINAL_KNOB_WIDTH = 53;

    static constexpr int ROW_OFFSET = 20;
    static constexpr int COL_OFFSET = 148;
    static constexpr int DISPLAY_START_X = 410;
    static constexpr int DISPLAY_START_Y = 44;
    static constexpr float DISPLAY_PLOT_YLIM_PADDING = .2f;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WahmbulanceProcessor &processorRef;
    Font displayFont;
    Font mainFont;

    unique_ptr<juce::Drawable> knobDrawable;
    unique_ptr<juce::Drawable> knobShadowDrawable;
    unique_ptr<juce::Drawable> knobTopShadowDrawable;
    unique_ptr<juce::Drawable> background;
    unique_ptr<Drawable> background_text;
    unique_ptr<Drawable> overlay;
    unique_ptr<Drawable> debugOverlay;

    CWPlotLookAndFeel plotLookAndFeel;
    WahmLookAndFeel lookAndFeel;

    std::vector<unique_ptr<KnobView>> knobViews;

    ComboBox filterTypeComboBox;
    ComboBoxParameterAttachment filterTypeAttachment;

    cmp::SemiLogX output_plot;
    std::vector<float> output_plot_freq_values;
    std::vector<std::vector<float>> output_plot_y_values;
    std::vector<std::vector<float>> output_plot_x_values;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WahmbulanceProcessorEditor)

    Rectangle<int> get_display_bounds(int row, int col) {
        return Rectangle<int>({ DISPLAY_START_X + col * COL_OFFSET, DISPLAY_START_Y + row * ROW_OFFSET, COL_OFFSET / 2, 16 });
    }
};
