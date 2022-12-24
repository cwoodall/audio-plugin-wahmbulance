#pragma once

#include "BinaryData.h"
#include <juce_audio_processors/juce_audio_processors.h>

enum CWColourIds {
    rotaryKnobArcFillColourId = 0x101,
    rotaryKnobArcFillReverseColourId = 0x102,
};

class CWLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CWLookAndFeel(const juce::Drawable &knobDrawable,
                  const juce::Drawable &knobShadowDrawable,
                  const juce::Drawable &knobTopShadowDrawable) : knob(knobDrawable.createCopy()),
                                                                 knobShadow(knobShadowDrawable.createCopy()),
                                                                 knobTopShadow(knobTopShadowDrawable.createCopy()) {
        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
        setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::black);
        setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
        setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
        setColour(juce::PopupMenu::ColourIds::backgroundColourId, juce::Colours::black);
        setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black);
        setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::white);
        setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::white);
        setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(0xffb7c4cf));
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xffa73ed7));
        setColour(CWColourIds::rotaryKnobArcFillColourId, juce::Colour(0xffa73ed7));
        setColour(CWColourIds::rotaryKnobArcFillReverseColourId, juce::Colour(0xff4b3ed7));
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff554742));
    }

    void drawRotarySlider(juce::Graphics &g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float,
                          float,
                          juce::Slider &slider) override {
        // Draw the background arcs
        float lengthPastKnobEnd = 5.0f; // Draw 5 x past knob size

        auto bounds = getSquareCenteredInRectangle(x + lengthPastKnobEnd, y + lengthPastKnobEnd, width - lengthPastKnobEnd * 2, height - lengthPastKnobEnd * 2);
        auto centre = bounds.getCentre();
        auto rotationAngle = (sliderPosProportional - 0.5f) * 1.5f * juce::MathConstants<float>::pi;
        auto startingAngle = (0 - 0.5f) * 1.5f * juce::MathConstants<float>::pi;

        juce::Path backgroundCircle;
        backgroundCircle.addCentredArc(centre.getX(), centre.getY(), width / 2, height / 2, 0, 0, (juce::MathConstants<float>::twoPi), false);

        auto fillType = juce::FillType();
        fillType.setColour(slider.findColour(juce::Slider::ColourIds::backgroundColourId));
        g.setFillType(fillType);
        g.fillPath(backgroundCircle);

        juce::Path arc;
        juce::Colour arcColour = findColour(juce::Slider::ColourIds::rotarySliderFillColourId);
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

        // juce::Colour color = slider.getValue() < 0.0f ? : juce::Slider::ColourIds::rotarySliderFillColourId;
        fillType = juce::FillType();
        fillType.setColour(arcColour);
        g.setFillType(fillType);
        g.fillPath(arc);

        //

        auto shiftX = bounds.getX() - static_cast<float>(x);
        auto shiftY = bounds.getY() - static_cast<float>(y);

        auto transform = juce::AffineTransform::scale(bounds.getWidth() / knob->getWidth())
                             .translated(shiftX, shiftY);
        auto rotationTransform = transform
                                     .rotated(rotationAngle, centre.x, centre.y);
        knob->setTransform(rotationTransform);
        knob->drawAt(g, static_cast<float>(x), static_cast<float>(y), 1.0f);

        knobTopShadow->setTransform(transform);
        knobTopShadow->drawAt(g, static_cast<float>(x), static_cast<float>(y), 1.f);
    }

    template <typename T>
    static juce::Rectangle<float> getSquareCenteredInRectangle(const juce::Rectangle<T> &rectangle) {
        auto edgeLength = std::min(rectangle.getWidth(), rectangle.getHeight());
        auto square = juce::Rectangle<T>(edgeLength, edgeLength).withCentre(rectangle.getCentre());
        return square.toFloat();
    }

    template <typename T>
    static juce::Rectangle<float> getSquareCenteredInRectangle(T x,
                                                               T y,
                                                               T width,
                                                               T height) {
        return getSquareCenteredInRectangle(juce::Rectangle<T>(x, y, width, height));
    }

private:
    std::unique_ptr<juce::Drawable> knob;
    std::unique_ptr<juce::Drawable> knobShadow;
    std::unique_ptr<juce::Drawable> knobTopShadow;
};