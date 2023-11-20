#include "plugin_editor.h"
#include "BinaryData.h"
#include "dsp_math.h"
#include "plugin_processor.h"
#include "wahm_look_and_feel.h"

#include <cmp_lookandfeel.h>
#include <cmp_plot.h>
#include <complex>
#include <memory>
#include <tuple>
#include <vector>

using namespace std;
using namespace juce;
using namespace cw::dsp;

//==============================================================================
WahmbulanceProcessorEditor::WahmbulanceProcessorEditor(WahmbulanceProcessor &p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      displayFont(Typeface::createSystemTypefaceFor(
          BinaryData::FiraMonoRegular_ttf,
          BinaryData::FiraMonoRegular_ttfSize)),
      mainFont(Typeface::createSystemTypefaceFor(
          BinaryData::FiraMonoRegular_ttf,
          BinaryData::FiraMonoRegular_ttfSize)),
      knobDrawable(Drawable::createFromImageData(
          BinaryData::knob_main_svg,
          BinaryData::knob_main_svgSize)),
      knobShadowDrawable(Drawable::createFromImageData(
          BinaryData::knob_shadow_svg,
          BinaryData::knob_shadow_svgSize)),
      knobTopShadowDrawable(Drawable::createFromImageData(
          BinaryData::knob_topshadow_svg,
          BinaryData::knob_topshadow_svgSize)),
      background(Drawable::createFromImageData(
          BinaryData::background_png,
          BinaryData::background_pngSize)),
      background_text(Drawable::createFromImageData(
          BinaryData::background_text_png,
          BinaryData::background_text_pngSize)),
      overlay(Drawable::createFromImageData(
          BinaryData::shadow_overlay_png,
          BinaryData::shadow_overlay_pngSize)),
      debugOverlay(Drawable::createFromImageData(
          BinaryData::debug_overlay_png,
          BinaryData::debug_overlay_pngSize)),
      plotLookAndFeel(),
      lookAndFeel(
          *knobDrawable,
          *knobShadowDrawable,
          *knobTopShadowDrawable),
      knobViews(),
      filterTypeComboBox(),
      filterTypeAttachment(
          *p.filterType,
          filterTypeComboBox),
      output_plot(),
      output_plot_freq_values(),
      output_plot_y_values(),
      output_plot_x_values() {
    knobViews.push_back(std::make_unique<KnobView>(
        *p.filterStartingFreqHz,
        "FREQ  (HZ):",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_0_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(0, 0),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox));

    knobViews.push_back(std::make_unique<KnobView>(
        *p.filterRangeHz,
        "RANGE(HZ):",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_1_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(1, 0),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox,
        true));

    knobViews.push_back(std::make_unique<KnobView>(
        *p.filterResonance,
        "RESN    :",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_2_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(2, 0),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox));

    knobViews.push_back(std::make_unique<KnobView>(
        *p.envelopeAttackMs,
        "ATK (MS):",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_0_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(0, 1),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox));

    knobViews.push_back(std::make_unique<KnobView>(
        *p.envelopeReleaseMs,
        "REL (MS):",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_1_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(1, 1),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox));

    knobViews.push_back(std::make_unique<KnobView>(
        *p.envelopeSensitivity,
        "SENSTY  :",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_2_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(2, 1),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox));

    knobViews.push_back(std::make_unique<KnobView>(
        *p.outputMix,
        "MIX     :",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_3_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(3, 0),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox));

    knobViews.push_back(std::make_unique<KnobView>(
        *p.outputGain,
        "GAIN  :",
        displayFont,
        Rectangle<int>({ PLUGIN_ORIGINAL_COLUMN_4_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT }),
        get_display_bounds(3, 1),
        juce::Slider::RotaryVerticalDrag,
        juce::Slider::NoTextBox));

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(PLUGIN_ORIGINAL_WIDTH_PX, PLUGIN_ORIGINAL_HEIGHT_PX);
    startTimerHz(UPDATE_RATE_HZ);

    setLookAndFeel(&lookAndFeel);

    for (auto &knob : knobViews) {
        addAndMakeVisible(*knob);
    }

    addAndMakeVisible(filterTypeComboBox);
    filterTypeComboBox.addItemList(
        { "Lowpass", "Bandpass", "Highpass" }, 1);

    filterTypeAttachment.sendInitialUpdate();
    // Setup display section
    displayFont.setHeight(14);

    addAndMakeVisible(output_plot);
    output_plot.setLookAndFeel(&plotLookAndFeel);
    output_plot.setColour(cmp::Plot::ColourIds::background_colour, juce::Colours::black);
    output_plot.setColour(cmp::Plot::ColourIds::frame_colour, juce::Colours::black);

    output_plot_freq_values = logspace<float>(1024, 1, 26000);

    output_plot_y_values.resize(3);
    output_plot_x_values.resize(3);
    for (size_t i = 0; i < output_plot_x_values.size(); i++) {
        output_plot_x_values[i] = output_plot_freq_values;
        output_plot_y_values[i] = second_order_filter_response<float>(output_plot_x_values[i], 100, 20, AdjustableBiquadFilter::Type::LOWPASS);
    }

    output_plot.setDownsamplingType(cmp::DownsamplingType::no_downsampling);

    output_plot.plot(output_plot_y_values, output_plot_x_values);
}
WahmbulanceProcessorEditor::~WahmbulanceProcessorEditor() {
}

//==============================================================================
void WahmbulanceProcessorEditor::timerCallback() {
    float start_freq = processorRef.filterStartingFreqHz->get();
    float range = processorRef.filterRangeHz->get();
    float q = processorRef.filterResonance->get();
    AdjustableBiquadFilter::Type filter_type = static_cast<AdjustableBiquadFilter::Type>(
        processorRef.filterType->getIndex());

    std::array<float, 3> cutoff_freqs = {
        start_freq,
        start_freq + range,
        processorRef.getCutoffAverageFreq()
    };

    clip(cutoff_freqs.begin(), cutoff_freqs.end(), 5.0f, 26000.0f);

    for (size_t i = 0; i < output_plot_y_values.size(); i++) {
        output_plot_y_values[i] = second_order_filter_response<float>(output_plot_freq_values, cutoff_freqs[i], q, filter_type);
    }

    // Rescale the output
    const auto max_y_val = std::max_element(
        output_plot_y_values[0].begin(),
        output_plot_y_values[0].end());
    auto y_lim_min = 0 - DISPLAY_PLOT_YLIM_PADDING;
    auto y_lim_max = *max_y_val + DISPLAY_PLOT_YLIM_PADDING;
    output_plot.yLim(y_lim_min, y_lim_max);
    output_plot.realTimePlot(output_plot_y_values);
}

void WahmbulanceProcessorEditor::paint(Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Draw the background
    auto bounds = getBounds();
    float scale = bounds.getWidth() / (float) background->getWidth();
    auto scaleTransform = AffineTransform::scale(scale);
    background->setTransform(scaleTransform);
    background->drawAt(g, 0, 0, 1);

    // Draw the background
    scale = bounds.getWidth() / (float) background_text->getWidth();
    scaleTransform = AffineTransform::scale(scale);
    background_text->setTransform(scaleTransform);
    background_text->drawAt(g, 0, 0, 1);
}

void WahmbulanceProcessorEditor::paintOverChildren(Graphics &g) {
    auto bounds = getBounds();
    float scale = bounds.getWidth() / (float) overlay->getWidth();
    auto scaleTransform = AffineTransform::scale(scale);
    overlay->setTransform(scaleTransform);
    overlay->drawAt(g, 0, 0, 1);

#ifdef DEBUG_UI
    // Draw the debug overlay which marks where all the components should be laid out
    scale = bounds.getWidth() / (float) debugOverlay->getWidth();
    scaleTransform = AffineTransform::scale(scale);
    debugOverlay->setTransform(scaleTransform);
    debugOverlay->drawAt(g, 0, 0, 1);
#endif
}

void WahmbulanceProcessorEditor::resized() {
    for (auto &knob : knobViews) {
        knob->setBounds(0, 0, PLUGIN_ORIGINAL_WIDTH_PX, PLUGIN_ORIGINAL_HEIGHT_PX);
    }
    filterTypeComboBox.setBounds(252, 24, 34 * 3, 44);

    output_plot.setBounds(DISPLAY_START_X - 30, DISPLAY_START_Y + ROW_OFFSET * 4, 296, 85);
}
