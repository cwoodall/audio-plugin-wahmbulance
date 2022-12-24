#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"
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

//==============================================================================
WahmbulanceProcessorEditor::WahmbulanceProcessorEditor(WahmbulanceProcessor &p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      filterStartingFreqHzAttachment(*p.filterStartingFreqHz, filterStartingFreqHzSlider),
      filterResonanceAttachment(*p.filterResonance, filterResonanceSlider),
      filterRangeHzAttachment(*p.filterRangeHz, filterRangeHzSlider),
      envelopeSensitivityAttachment(*p.envelopeSensitivity, envelopeSensitivitySlider),
      envelopeAttackSAttachment(*p.envelopeAttackS, envelopeAttackSSlider),
      envelopeDecaySAttachment(*p.envelopeDecayS, envelopeDecaySSlider),
      outputGainAttachment(*p.outputGain, outputGainSlider),
      outputMixAttachment(*p.outputMix, outputMixSlider),
      componentPositions { {
          { filterStartingFreqHzSlider, { PLUGIN_ORIGINAL_COLUMN_0_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { filterRangeHzSlider, { PLUGIN_ORIGINAL_COLUMN_1_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { filterResonanceSlider, { PLUGIN_ORIGINAL_COLUMN_2_PX, PLUGIN_ORIGINAL_ROW_0_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { envelopeAttackSSlider, { PLUGIN_ORIGINAL_COLUMN_0_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
          { envelopeDecaySSlider, { PLUGIN_ORIGINAL_COLUMN_1_PX, PLUGIN_ORIGINAL_ROW_1_PX, PLUGIN_ORIGINAL_KNOB_WIDTH, PLUGIN_ORIGINAL_KNOB_HEIGHT } },
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

      lookAndFeel(*knobDrawable, *knobShadowDrawable, *knobTopShadowDrawable) {
    ignoreUnused(processorRef);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(700, 250);

    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(filterStartingFreqHzSlider);

    addAndMakeVisible(filterResonanceSlider);

    addAndMakeVisible(filterRangeHzSlider);
    outputMixSlider.getProperties().set("rotaryKnobFromMidPoint", static_cast<bool>(true));

    addAndMakeVisible(envelopeSensitivitySlider);

    addAndMakeVisible(envelopeAttackSSlider);

    addAndMakeVisible(envelopeDecaySSlider);

    addAndMakeVisible(outputGainSlider);

    addAndMakeVisible(outputMixSlider);
    outputMixSlider.getProperties().set("rotaryKnobFromMidPoint", static_cast<bool>(true));
}

WahmbulanceProcessorEditor::~WahmbulanceProcessorEditor() {
}

//==============================================================================
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
}