#include "PluginEditor.h"
#include "BinaryData.h"
#include "CWDspMath.h"
#include "PluginProcessor.h"
#include <cmp_lookandfeel.h>
#include <cmp_plot.h>
#include <complex>
#include <tuple>
#include <vector>

using namespace std;
using namespace juce;

#define PLUGIN_ORIGINAL_WIDTH_PX (700)
#define PLUGIN_ORIGINAL_HEIGHT_PX (250)
#define PLUGIN_ORIGINAL_ROW_0_PX (17)
#define PLUGIN_ORIGINAL_ROW_1_PX (116)
#define PLUGIN_ORIGINAL_COLUMN_0_PX (30)
#define PLUGIN_ORIGINAL_COLUMN_1_PX (100)
#define PLUGIN_ORIGINAL_COLUMN_2_PX (170)
#define PLUGIN_ORIGINAL_COLUMN_3_PX (258)
#define PLUGIN_ORIGINAL_COLUMN_4_PX (328)
#define PLUGIN_ORIGINAL_KNOB_HEIGHT (53)
#define PLUGIN_ORIGINAL_KNOB_WIDTH (53)

class CWPlotLookAndFeel : public cmp::PlotLookAndFeel {
    void drawBackground(juce::Graphics &g,
                        const juce::Rectangle<int> &bound) override {
        g.setColour(juce::Colours::black);
    };

    void drawFrame(juce::Graphics &g, const juce::Rectangle<int> bounds) override {}

    void drawGridLine(juce::Graphics &g, const cmp::GridLine &grid_line, const bool grid_on) override {}

    void drawGridLabels(juce::Graphics &g, const cmp::LabelVector &x_axis_labels, const cmp::LabelVector &y_axis_labels) override {}

    void drawLegendBackground(juce::Graphics &g,
                              const juce::Rectangle<int> &legend_bound) override {}

    void drawSpread(juce::Graphics &g, const cmp::GraphLine *first_graph, const cmp::GraphLine *second_graph, const juce::Colour &spread_colour) override {}

    void drawTraceLabel(juce::Graphics &g, const cmp::Label &x_label, const cmp::Label &y_label, const juce::Rectangle<int> bound) override {}
};
/* Get vector of sine wave. */
template <class ValueType>
std::vector<ValueType> generate_frequencies(const std::size_t length,
                                            ValueType min,
                                            ValueType max) {
    std::vector<ValueType> retval(length);

    auto start = log10(min);
    auto stop = log10(max);
    auto dx = (stop - start) / length;

    auto last_x = min;
    for (auto &x : retval) {
        x = pow(10.0f, log10(last_x) + dx);
        last_x = x;
    }
    return retval;
};

template <typename ValueType>
ValueType second_order_filter_gain(ValueType freq, ValueType fc, ValueType q, VariableFreqBiquadFilter::Type type) {
    std::complex<ValueType> sn = std::complex<ValueType>(1.0i) * freq / fc;
    switch (type) {
        case VariableFreqBiquadFilter::Type::LOWPASS: {
            std::complex<ValueType> value = 1.0f / (sn * sn + sn / q + 1.0f);
            return static_cast<ValueType>(std::abs(value));
        }
        case VariableFreqBiquadFilter::Type::BANDPASS: {
            std::complex<ValueType> value = (sn / q) / (sn * sn + sn / q + 1.0f);
            return static_cast<ValueType>(std::abs(value));
        }
        case VariableFreqBiquadFilter::Type::HIGHPASS: {
            std::complex<ValueType> value = (sn * sn) / (sn * sn + sn / q + 1.0f);
            return static_cast<ValueType>(std::abs(value));
        }
    }
}

template <typename ValueType, typename Iter = typename std::vector<ValueType>::const_iterator>
std::vector<ValueType> generate_frequency_gains(std::vector<ValueType> const &freqs,
                                                ValueType fc,
                                                ValueType resonance,
                                                VariableFreqBiquadFilter::Type type) {
    std::vector<float> retval(freqs.size());

    size_t idx = 0;
    for (const auto freq : freqs) {
        retval[idx++] = second_order_filter_gain(freq, fc, resonance, type);
    }

    return retval;
};

//==============================================================================
WahmbulanceProcessorEditor::WahmbulanceProcessorEditor(WahmbulanceProcessor &p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      filterStartingFreqHzAttachment(*p.filterStartingFreqHz, filterStartingFreqHzSlider),
      filterResonanceAttachment(*p.filterResonance, filterResonanceSlider),
      filterRangeHzAttachment(*p.filterRangeHz, filterRangeHzSlider),
      envelopeSensitivityAttachment(*p.envelopeSensitivity, envelopeSensitivitySlider),
      envelopeAttackMsAttachment(*p.envelopeAttackMs, envelopeAttackMsSlider),
      envelopeReleaseMsAttachment(*p.envelopeReleaseMs, envelopeReleaseMsSlider),
      outputGainAttachment(*p.outputGain, outputGainSlider),
      outputMixAttachment(*p.outputMix, outputMixSlider),
      filterTypeAttachment(*p.filterType, filterTypeComboBox),
      componentPositions { {
          { filterStartingFreqHzSlider, { PLUGIN_ORIGINAL_COLUMN_0_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { filterRangeHzSlider, { PLUGIN_ORIGINAL_COLUMN_1_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { filterResonanceSlider, { PLUGIN_ORIGINAL_COLUMN_2_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { envelopeAttackMsSlider, { PLUGIN_ORIGINAL_COLUMN_0_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { envelopeReleaseMsSlider, { PLUGIN_ORIGINAL_COLUMN_1_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { envelopeSensitivitySlider, { PLUGIN_ORIGINAL_COLUMN_2_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { outputMixSlider, { PLUGIN_ORIGINAL_COLUMN_3_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { outputGainSlider, { PLUGIN_ORIGINAL_COLUMN_4_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
      } },
      knobDrawable(Drawable::createFromImageData(BinaryData::knob_main_svg, BinaryData::knob_main_svgSize)),
      knobShadowDrawable(Drawable::createFromImageData(BinaryData::knob_shadow_svg, BinaryData::knob_shadow_svgSize)),
      knobTopShadowDrawable(Drawable::createFromImageData(BinaryData::knob_topshadow_svg, BinaryData::knob_topshadow_svgSize)),
      background(Drawable::createFromImageData(BinaryData::background_png, BinaryData::background_pngSize)),
      background_text(Drawable::createFromImageData(BinaryData::background_text_png, BinaryData::background_text_pngSize)),
      overlay(Drawable::createFromImageData(BinaryData::shadow_overlay_png, BinaryData::shadow_overlay_pngSize)),
      debugOverlay(Drawable::createFromImageData(BinaryData::debug_overlay_png, BinaryData::debug_overlay_pngSize)),
      displayFont(Typeface::createSystemTypefaceFor(BinaryData::FiraMonoRegular_ttf,
                                                    BinaryData::FiraMonoRegular_ttfSize)),
      mainFont(Typeface::createSystemTypefaceFor(BinaryData::FiraMonoRegular_ttf,
                                                 BinaryData::FiraMonoRegular_ttfSize)),

      lookAndFeel(*knobDrawable, *knobShadowDrawable, *knobTopShadowDrawable) {
    ignoreUnused(processorRef);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(700, 250);
    startTimerHz(30);

    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(filterStartingFreqHzSlider);
    filterStartingFreqHzSlider.textFromValueFunction = [](double x) -> juce::String {
        return String::formatted("% 6.0f", x);
    };
    filterStartingFreqHzSlider.onValueChange = [this]() {
        filterStartingFreqHzValueLabel.setText(filterStartingFreqHzSlider.getTextFromValue(filterStartingFreqHzSlider.getValue()), NotificationType::dontSendNotification);
    };

    addAndMakeVisible(filterResonanceSlider);
    filterResonanceSlider.onValueChange = [this]() {
        filterResonanceValueLabel.setText(filterResonanceSlider.getTextFromValue(filterResonanceSlider.getValue()), NotificationType::dontSendNotification);
    };
    addAndMakeVisible(filterRangeHzSlider);
    filterRangeHzSlider.onValueChange = [this]() {
        filterRangeHzValueLabel.setText(filterRangeHzSlider.getTextFromValue(filterRangeHzSlider.getValue()), NotificationType::dontSendNotification);
    };
    filterRangeHzSlider.textFromValueFunction = [](double x) -> juce::String {
        return String::formatted("%+6.0f", x);
    };

    filterRangeHzSlider.getProperties().set("rotaryKnobFromMidPoint", static_cast<bool>(true));
    addAndMakeVisible(envelopeSensitivitySlider);
    envelopeSensitivitySlider.onValueChange = [this]() {
        envelopeSensitivityValueLabel.setText(envelopeSensitivitySlider.getTextFromValue(envelopeSensitivitySlider.getValue()), NotificationType::dontSendNotification);
    };
    addAndMakeVisible(envelopeAttackMsSlider);
    envelopeAttackMsSlider.onValueChange = [this]() {
        envelopeAttackMsValueLabel.setText(envelopeAttackMsSlider.getTextFromValue(envelopeAttackMsSlider.getValue()), NotificationType::dontSendNotification);
    };
    addAndMakeVisible(envelopeReleaseMsSlider);
    envelopeReleaseMsSlider.onValueChange = [this]() {
        envelopeReleaseMsValueLabel.setText(envelopeReleaseMsSlider.getTextFromValue(envelopeReleaseMsSlider.getValue()), NotificationType::dontSendNotification);
    };
    addAndMakeVisible(outputGainSlider);
    outputGainSlider.onValueChange = [this]() {
        outputGainValueLabel.setText(outputGainSlider.getTextFromValue(outputGainSlider.getValue()), NotificationType::dontSendNotification);
    };
    addAndMakeVisible(outputMixSlider);
    outputMixSlider.onValueChange = [this]() {
        outputMixValueLabel.setText(outputMixSlider.getTextFromValue(outputMixSlider.getValue()), NotificationType::dontSendNotification);
    };
    outputMixSlider.getProperties().set("rotaryKnobFromMidPoint", static_cast<bool>(true));

    addAndMakeVisible(filterTypeComboBox);
    filterTypeComboBox.addItemList(
        { "Lowpass", "Bandpass", "Highpass" }, 1);

    filterTypeAttachment.sendInitialUpdate();
    // Setup display section
    displayFont.setHeight(14);

    addAndMakeVisible(filterStartingFreqHzValueLabel);
    filterStartingFreqHzValueLabel.setFont(displayFont);
    filterStartingFreqHzValueLabel.setText(filterStartingFreqHzSlider.getTextFromValue(filterStartingFreqHzSlider.getValue()), dontSendNotification);
    filterStartingFreqHzValueLabel.setEditable(true);
    filterStartingFreqHzValueLabel.onTextChange = [this]() {
        filterStartingFreqHzSlider.setValue(filterStartingFreqHzSlider.getValueFromText(filterStartingFreqHzValueLabel.getText()));
    };

    addAndMakeVisible(filterResonanceValueLabel);
    filterResonanceValueLabel.setFont(displayFont);
    filterResonanceValueLabel.setText(filterResonanceSlider.getTextFromValue(filterResonanceSlider.getValue()), dontSendNotification);
    filterResonanceValueLabel.setEditable(true);
    filterResonanceValueLabel.onTextChange = [this]() {
        filterResonanceSlider.setValue(filterResonanceSlider.getValueFromText(filterResonanceValueLabel.getText()));
    };

    addAndMakeVisible(filterRangeHzValueLabel);
    filterRangeHzValueLabel.setFont(displayFont);
    filterRangeHzValueLabel.setText(filterRangeHzSlider.getTextFromValue(filterRangeHzSlider.getValue()), dontSendNotification);
    filterRangeHzValueLabel.setEditable(true);
    filterRangeHzValueLabel.onTextChange = [this]() {
        filterRangeHzSlider.setValue(filterRangeHzSlider.getValueFromText(filterRangeHzValueLabel.getText()));
    };

    addAndMakeVisible(envelopeSensitivityValueLabel);
    envelopeSensitivityValueLabel.setFont(displayFont);
    envelopeSensitivityValueLabel.setText(envelopeSensitivitySlider.getTextFromValue(envelopeSensitivitySlider.getValue()), dontSendNotification);
    envelopeSensitivityValueLabel.setEditable(true);
    envelopeSensitivityValueLabel.onTextChange = [this]() {
        envelopeSensitivitySlider.setValue(envelopeSensitivitySlider.getValueFromText(envelopeSensitivityValueLabel.getText()));
    };

    addAndMakeVisible(envelopeAttackMsValueLabel);
    envelopeAttackMsValueLabel.setFont(displayFont);
    envelopeAttackMsValueLabel.setText(envelopeAttackMsSlider.getTextFromValue(envelopeAttackMsSlider.getValue()), dontSendNotification);
    envelopeAttackMsValueLabel.setEditable(true);
    envelopeAttackMsValueLabel.onTextChange = [this]() {
        envelopeAttackMsSlider.setValue(envelopeAttackMsSlider.getValueFromText(envelopeAttackMsValueLabel.getText()));
    };

    addAndMakeVisible(envelopeReleaseMsValueLabel);
    envelopeReleaseMsValueLabel.setFont(displayFont);
    envelopeReleaseMsValueLabel.setText(envelopeReleaseMsSlider.getTextFromValue(envelopeReleaseMsSlider.getValue()), dontSendNotification);
    envelopeReleaseMsValueLabel.setEditable(true);
    envelopeReleaseMsValueLabel.onTextChange = [this]() {
        envelopeReleaseMsSlider.setValue(envelopeReleaseMsSlider.getValueFromText(envelopeReleaseMsValueLabel.getText()));
    };

    addAndMakeVisible(outputGainValueLabel);
    outputGainValueLabel.setFont(displayFont);
    outputGainValueLabel.setText(outputGainSlider.getTextFromValue(outputGainSlider.getValue()), dontSendNotification);
    outputGainValueLabel.setEditable(true);
    outputGainValueLabel.onTextChange = [this]() {
        outputGainSlider.setValue(outputGainSlider.getValueFromText(outputGainValueLabel.getText()));
    };

    addAndMakeVisible(outputMixValueLabel);
    outputMixValueLabel.setFont(displayFont);
    outputMixValueLabel.setText(outputMixSlider.getTextFromValue(outputMixSlider.getValue()), dontSendNotification);
    outputMixValueLabel.setEditable(true);
    outputMixValueLabel.onTextChange = [this]() {
        outputMixSlider.setValue(outputMixSlider.getValueFromText(outputMixValueLabel.getText()));
    };

    addAndMakeVisible(filterStartingFreqHzLabel);
    filterStartingFreqHzLabel.setText("FREQ (HZ):", NotificationType::dontSendNotification);
    filterStartingFreqHzLabel.setFont(displayFont);
    addAndMakeVisible(filterRangeHzLabel);
    filterRangeHzLabel.setText("RANGE(HZ):", NotificationType::dontSendNotification);
    filterRangeHzLabel.setFont(displayFont);
    addAndMakeVisible(filterResonanceLabel);
    filterResonanceLabel.setText("RESN  :", NotificationType::dontSendNotification);
    filterResonanceLabel.setFont(displayFont);

    addAndMakeVisible(envelopeSensitivityLabel);
    envelopeSensitivityLabel.setText("SENSTY :", NotificationType::dontSendNotification);
    envelopeSensitivityLabel.setFont(displayFont);
    addAndMakeVisible(envelopeAttackMsLabel);
    envelopeAttackMsLabel.setText("ATK (MS):", NotificationType::dontSendNotification);
    envelopeAttackMsLabel.setFont(displayFont);
    addAndMakeVisible(envelopeReleaseMsLabel);
    envelopeReleaseMsLabel.setText("REL (MS):", NotificationType::dontSendNotification);
    envelopeReleaseMsLabel.setFont(displayFont);
    addAndMakeVisible(outputGainLabel);
    outputGainLabel.setText("GAIN:", NotificationType::dontSendNotification);
    outputGainLabel.setFont(displayFont);
    addAndMakeVisible(outputMixLabel);
    outputMixLabel.setText("MIX:", NotificationType::dontSendNotification);
    outputMixLabel.setFont(displayFont);

    addAndMakeVisible(output_plot);
    static CWPlotLookAndFeel plot_look; // TODO(cw):Fix this
    output_plot.setLookAndFeel(&plot_look);
    output_plot.setColour(cmp::Plot::ColourIds::background_colour, juce::Colours::black);
    output_plot.setColour(cmp::Plot::ColourIds::frame_colour, juce::Colours::black);

    output_plot_freq_values = generate_frequencies<float>(1024, 1, 26000);

    output_plot_x_values = {
        output_plot_freq_values, output_plot_freq_values, output_plot_freq_values
    };

    output_plot_y_values = {
        generate_frequency_gains<float>(output_plot_freq_values, 100, 20, VariableFreqBiquadFilter::Type::LOWPASS),
        generate_frequency_gains<float>(output_plot_freq_values, 100, 20, VariableFreqBiquadFilter::Type::LOWPASS),
        generate_frequency_gains<float>(output_plot_freq_values, 100, 20, VariableFreqBiquadFilter::Type::LOWPASS)
    };
    output_plot.setDownsamplingType(cmp::DownsamplingType::no_downsampling);

    output_plot.plot(output_plot_y_values, output_plot_x_values);
}
WahmbulanceProcessorEditor::~WahmbulanceProcessorEditor() {
}

//==============================================================================
void WahmbulanceProcessorEditor::timerCallback() {
    auto start_freq = this->processorRef.filterStartingFreqHz->get();
    auto range = this->processorRef.filterRangeHz->get();
    auto q = this->processorRef.filterResonance->get();
    auto stop_freq = clip<float>(start_freq + range, 5, 20000);
    auto average_autowah_freq_envelope = processorRef.getCutoffAverageFreq();
    auto filter_type = static_cast<VariableFreqBiquadFilter::Type>(
        processorRef.filterType->getIndex());
    output_plot_y_values[0] = generate_frequency_gains<float>(output_plot_freq_values, start_freq, q, filter_type);
    output_plot_y_values[1] = generate_frequency_gains<float>(output_plot_freq_values, stop_freq, q, filter_type);
    output_plot_y_values[2] = generate_frequency_gains<float>(output_plot_freq_values, average_autowah_freq_envelope, q, filter_type);

    auto max_values = std::max_element(output_plot_y_values[0].begin(), output_plot_y_values[0].end());
    const float YLIM_PADDING = .2f;
    output_plot.yLim(0 - YLIM_PADDING, *max_values + YLIM_PADDING);
    output_plot.realTimePlot({
        output_plot_y_values[0],
        output_plot_y_values[1],
        output_plot_y_values[2]
    });
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

    // scale = bounds.getWidth() / (float) debugOverlay->getWidth();
    // scaleTransform = AffineTransform::scale(scale);
    // debugOverlay->setTransform(scaleTransform);
    // debugOverlay->drawAt(g, 0, 0, 1);
}

void WahmbulanceProcessorEditor::resized() {
    for (auto [component, bounds] : componentPositions) {
        auto [x, y, w, h] = bounds;
        // This is generally where you'll want to lay out the positions of any
        // subcomponents in your editor..
        component.setBounds(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h));
    }

    filterTypeComboBox.setBounds(252, 24, 34 * 3, 44);

    const int VALUE_OFFSET = 66;
    const int ROW_OFFSET = 20;
    const int COL_OFFSET = 148;
    const int DISPLAY_START_X = 410;
    const int DISPLAY_START_Y = 44;

    filterStartingFreqHzLabel.setBounds(DISPLAY_START_X, DISPLAY_START_Y, COL_OFFSET / 2, 16);
    filterStartingFreqHzValueLabel.setBounds(DISPLAY_START_X + VALUE_OFFSET, DISPLAY_START_Y, COL_OFFSET / 2, 16);
    filterRangeHzLabel.setBounds(DISPLAY_START_X, DISPLAY_START_Y + ROW_OFFSET, COL_OFFSET / 2, 16);
    filterRangeHzValueLabel.setBounds(DISPLAY_START_X + VALUE_OFFSET, DISPLAY_START_Y + ROW_OFFSET, COL_OFFSET / 2, 16);
    filterResonanceLabel.setBounds(DISPLAY_START_X, DISPLAY_START_Y + ROW_OFFSET * 2, COL_OFFSET / 2, 16);
    filterResonanceValueLabel.setBounds(DISPLAY_START_X + VALUE_OFFSET, DISPLAY_START_Y + ROW_OFFSET * 2, COL_OFFSET / 2, 16);
    outputGainLabel.setBounds(DISPLAY_START_X, DISPLAY_START_Y + ROW_OFFSET * 3, COL_OFFSET / 2, 16);
    outputGainValueLabel.setBounds(DISPLAY_START_X + VALUE_OFFSET, DISPLAY_START_Y + ROW_OFFSET * 3, COL_OFFSET / 2, 16);

    envelopeSensitivityLabel.setBounds(DISPLAY_START_X + COL_OFFSET, DISPLAY_START_Y, COL_OFFSET / 2, 16);
    envelopeSensitivityValueLabel.setBounds(DISPLAY_START_X + VALUE_OFFSET + COL_OFFSET, DISPLAY_START_Y, COL_OFFSET / 2, 16);
    envelopeAttackMsLabel.setBounds(DISPLAY_START_X + COL_OFFSET, DISPLAY_START_Y + ROW_OFFSET, COL_OFFSET / 2, 16);
    envelopeAttackMsValueLabel.setBounds(DISPLAY_START_X + VALUE_OFFSET + COL_OFFSET, DISPLAY_START_Y + ROW_OFFSET, COL_OFFSET / 2, 16);
    envelopeReleaseMsLabel.setBounds(DISPLAY_START_X + COL_OFFSET, DISPLAY_START_Y + ROW_OFFSET * 2, COL_OFFSET / 2, 16);
    envelopeReleaseMsValueLabel.setBounds(DISPLAY_START_X + VALUE_OFFSET + COL_OFFSET, DISPLAY_START_Y + ROW_OFFSET * 2, COL_OFFSET / 2, 16);
    outputMixLabel.setBounds(DISPLAY_START_X + COL_OFFSET, DISPLAY_START_Y + ROW_OFFSET * 3, COL_OFFSET / 2, 16);
    outputMixValueLabel.setBounds(DISPLAY_START_X + COL_OFFSET + VALUE_OFFSET, DISPLAY_START_Y + ROW_OFFSET * 3, COL_OFFSET / 2, 16);

    output_plot.setBounds(DISPLAY_START_X - 30, DISPLAY_START_Y + ROW_OFFSET * 4, 296, 85);
}