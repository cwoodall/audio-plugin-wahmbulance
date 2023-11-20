#pragma once

#include "BinaryData.h"
#include <cmp_lookandfeel.h>
#include <cmp_plot.h>
#include <juce_audio_processors/juce_audio_processors.h>

using namespace juce;

enum CWColourIds {
    rotaryKnobArcFillColourId = 0x101,
    rotaryKnobArcFillReverseColourId = 0x102,
};

class WahmLookAndFeel : public LookAndFeel_V4 {
public:
    WahmLookAndFeel(const Drawable &knobDrawable,
                    const Drawable &knobShadowDrawable,
                    const Drawable &knobTopShadowDrawable) : knob(knobDrawable.createCopy()),
                                                             knobShadow(knobShadowDrawable.createCopy()),
                                                             knobTopShadow(knobTopShadowDrawable.createCopy()) {
        setColour(TextButton::ColourIds::buttonColourId, Colours::black);
        setColour(ToggleButton::ColourIds::textColourId, Colours::black);
        setColour(ToggleButton::ColourIds::tickColourId, Colours::black);
        setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::black);
        setColour(PopupMenu::ColourIds::backgroundColourId, Colours::black);
        setColour(ComboBox::ColourIds::backgroundColourId, Colours::black);
        setColour(ComboBox::ColourIds::arrowColourId, Colours::white);
        setColour(ComboBox::ColourIds::outlineColourId, Colours::white);
        setColour(Slider::ColourIds::backgroundColourId, Colour(0xffb7c4cf));
        setColour(Slider::ColourIds::rotarySliderFillColourId, Colour(0xffa73ed7));
        setColour(CWColourIds::rotaryKnobArcFillColourId, Colour(0xffa73ed7));
        setColour(CWColourIds::rotaryKnobArcFillReverseColourId, Colour(0xff4b3ed7));
        setColour(ResizableWindow::backgroundColourId, Colour(0xff554742));
    }

    Font getComboBoxFont(ComboBox &) override {
        return getCommonMenuFont();
    }

    Font getPopupMenuFont() override {
        return getCommonMenuFont();
    }

    void drawRotarySlider(Graphics &g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float,
                          float,
                          Slider &slider) override {
        // Draw the background arcs
        float lengthPastKnobEnd = 5.0f; // Draw 5 x past knob size

        auto bounds = getSquareCenteredInRectangle(x + lengthPastKnobEnd, y + lengthPastKnobEnd, width - lengthPastKnobEnd * 2, height - lengthPastKnobEnd * 2);
        auto centre = bounds.getCentre();
        auto rotationAngle = (sliderPosProportional - 0.5f) * 1.5f * MathConstants<float>::pi;
        auto startingAngle = (0 - 0.5f) * 1.5f * MathConstants<float>::pi;

        Path backgroundCircle;
        backgroundCircle.addCentredArc(centre.getX(), centre.getY(), width / 2, height / 2, 0, 0, (MathConstants<float>::twoPi), false);

        auto fillType = FillType();
        fillType.setColour(slider.findColour(Slider::ColourIds::backgroundColourId));
        g.setFillType(fillType);
        g.fillPath(backgroundCircle);

        Path arc;
        Colour arcColour = findColour(Slider::ColourIds::rotarySliderFillColourId);
        if (slider.getProperties().contains("rotaryKnobFromMidPoint")) {
            arc.startNewSubPath(centre.getX(), centre.getY());
            arc.addCentredArc(centre.getX(), centre.getY(), width / 2, height / 2, 0, 0, rotationAngle, false);
            arc.closeSubPath(); // close the subpath with a line back to (10, 10)
            arcColour = rotationAngle < 0 ? findColour(CWColourIds::rotaryKnobArcFillReverseColourId) : findColour(CWColourIds::rotaryKnobArcFillColourId);
        } else {
            arc.startNewSubPath(centre.getX(), centre.getY());
            arc.addCentredArc(centre.getX(), centre.getY(), width / 2, height / 2, 0, startingAngle, rotationAngle, false);
            arc.closeSubPath(); // close the subpath with a line back to (10, 10)
        }

        // Colour color = slider.getValue() < 0.0f ? : Slider::ColourIds::rotarySliderFillColourId;
        fillType = FillType();
        fillType.setColour(arcColour);
        g.setFillType(fillType);
        g.fillPath(arc);

        //

        auto shiftX = bounds.getX() - static_cast<float>(x);
        auto shiftY = bounds.getY() - static_cast<float>(y);

        auto transform = AffineTransform::scale(bounds.getWidth() / knob->getWidth())
                             .translated(shiftX, shiftY);
        auto rotationTransform = transform
                                     .rotated(rotationAngle, centre.x, centre.y);
        knob->setTransform(rotationTransform);
        knob->drawAt(g, static_cast<float>(x), static_cast<float>(y), 1.0f);

        knobTopShadow->setTransform(transform);
        knobTopShadow->drawAt(g, static_cast<float>(x), static_cast<float>(y), 1.f);
    }

    template <typename T>
    static Rectangle<float> getSquareCenteredInRectangle(const Rectangle<T> &rectangle) {
        auto edgeLength = std::min(rectangle.getWidth(), rectangle.getHeight());
        auto square = Rectangle<T>(edgeLength, edgeLength).withCentre(rectangle.getCentre());
        return square.toFloat();
    }

    template <typename T>
    static Rectangle<float> getSquareCenteredInRectangle(T x,
                                                         T y,
                                                         T width,
                                                         T height) {
        return getSquareCenteredInRectangle(Rectangle<T>(x, y, width, height));
    }

private:
    std::unique_ptr<Drawable> knob;
    std::unique_ptr<Drawable> knobShadow;
    std::unique_ptr<Drawable> knobTopShadow;

    Font getCommonMenuFont() {
        auto tempFont = Font(Typeface::createSystemTypefaceFor(BinaryData::FiraMonoRegular_ttf, BinaryData::FiraMonoRegular_ttfSize));
        tempFont.setHeight(14);
        return tempFont;
    }
};

class CWPlotLookAndFeel : public cmp::PlotLookAndFeel {
    void drawBackground(juce::Graphics &g,
                        const juce::Rectangle<int> &bound) override {
        (void) bound;

        g.setColour(juce::Colours::black);
    }

    void drawFrame(juce::Graphics &g, const juce::Rectangle<int> bounds) override {
        (void) (bounds);
        (void) (g);
    }

    void drawGridLine(juce::Graphics &g, const cmp::GridLine &grid_line, const bool grid_on) override {
        (void) (g);
        (void) (grid_line);
        (void) (grid_on);
    }

    void drawGridLabels(juce::Graphics &g, const cmp::LabelVector &x_axis_labels, const cmp::LabelVector &y_axis_labels) override {
        (void) g;
        (void) x_axis_labels;
        (void) y_axis_labels;
    }

    void drawLegendBackground(juce::Graphics &g,
                              const juce::Rectangle<int> &legend_bound) override {
        (void) g;
        (void) legend_bound;
    }

    void drawSpread(juce::Graphics &g, const cmp::GraphLine *first_graph, const cmp::GraphLine *second_graph, const juce::Colour &spread_colour) override {
        (void) (g);
        (void) (first_graph);
        (void) (second_graph);
        (void) (spread_colour);
    }

    void drawTraceLabel(juce::Graphics &g, const cmp::Label &x_label, const cmp::Label &y_label, const juce::Rectangle<int> bound) override {
        (void) (g);
        (void) (x_label);
        (void) (y_label);
        (void) (bound);
    }
};
