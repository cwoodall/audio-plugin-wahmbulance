#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class CWLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CWLookAndFeel(const juce::Drawable &knobDrawable,
                  const juce::Drawable &knobShadowDrawable,
                  const juce::Drawable &knobTopShadowDrawable) : knob(knobDrawable.createCopy()),
                                                                 knobShadow(knobShadowDrawable.createCopy()),
                                                                 knobTopShadow(knobTopShadowDrawable.createCopy()) {
        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
        setColour(juce::PopupMenu::ColourIds::backgroundColourId, juce::Colours::black);
        setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black);
        setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::white);
        setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::white);
    }

    void drawRotarySlider(juce::Graphics &g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float,
                          float,
                          juce::Slider &) override {
        auto bounds = getSquareCenteredInRectangle(x, y, width, height);
        auto centre = bounds.getCentre();
        auto shiftX = bounds.getX() - static_cast<float>(x);
        auto shiftY = bounds.getY() - static_cast<float>(y);

        auto shadowTransform = juce::AffineTransform::scale(bounds.getWidth() / knob->getWidth())
                                   .translated(shiftX, shiftY);
        knobShadow->setTransform(shadowTransform);
        knobShadow->drawAt(g, static_cast<float>(x), static_cast<float>(y), 1.f);
        auto rotationAngle = (sliderPosProportional - 0.5f) * 1.5f * juce::MathConstants<float>::pi;
        auto transform = juce::AffineTransform::scale(bounds.getWidth() / knob->getWidth())
                             .translated(shiftX, shiftY)
                             .rotated(rotationAngle, centre.x, centre.y);
        knob->setTransform(transform);
        knob->drawAt(g, static_cast<float>(x), static_cast<float>(y), 1.0f);

        knobTopShadow->setTransform(shadowTransform);
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

    void drawComboBox(juce::Graphics &g,
                      int width,
                      int height,
                      bool,
                      int,
                      int,
                      int,
                      int,
                      juce::ComboBox &box) override {
        juce::Rectangle<int> bounds(0, 0, width, height);

        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawRect(bounds);

        juce::Rectangle<int> arrowZone(width - 20, 0, 15, height);
        juce::Path path;
        path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
        path.lineTo(static_cast<float>(arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
        path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType(1.5f));
    }

    void drawButtonBackground(juce::Graphics &g,
                              juce::Button &button,
                              const juce::Colour &backgroundColour,
                              bool,
                              bool) override {
        g.fillAll(backgroundColour);
        g.setColour(juce::Colours::white);
        g.drawRect(button.getLocalBounds());
    }

private:
    std::unique_ptr<juce::Drawable> knob;
    std::unique_ptr<juce::Drawable> knobShadow;
    std::unique_ptr<juce::Drawable> knobTopShadow;
};