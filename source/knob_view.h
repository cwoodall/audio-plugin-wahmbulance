#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

using namespace juce;

class KnobView : public Component {
public:
    static constexpr int VALUE_OFFSET = 66;

    KnobView(AudioParameterFloat &parameter,
             const std::string_view label_str,
             const Font &display_font,
             Rectangle<int> slider_position,
             Rectangle<int> label_position,
             Slider::SliderStyle style,
             Slider::TextEntryBoxPosition textBoxPosition,
             bool middle = false) : slider_(style, textBoxPosition),
                                    slider_position_(slider_position),
                                    label_position_(label_position),
                                    attachment_(parameter, slider_) {
        setInterceptsMouseClicks(false, true);
        addAndMakeVisible(slider_);
        slider_.onValueChange = [this] {
            view_label_.setText(String(slider_.getValue()), NotificationType::dontSendNotification);
        };

        if (middle) {
            slider_.getProperties().set("rotaryKnobFromMidPoint", static_cast<bool>(true));
        }

        addAndMakeVisible(label_);
        label_.setText(label_str.data(), NotificationType::dontSendNotification);
        label_.setFont(display_font);

        addAndMakeVisible(view_label_);
        view_label_.setText(String(slider_.getValue()), NotificationType::dontSendNotification);
        view_label_.setFont(display_font);
        view_label_.setEditable(true);
        view_label_.onTextChange = [this]() {
            slider_.setValue(slider_.getValueFromText(view_label_.getText()));
        };
    }

    void resized() {
        slider_.setBounds(slider_position_);
        label_.setBounds(label_position_);
        auto value_position = label_position_.translated(VALUE_OFFSET, 0);
        view_label_.setBounds(value_position);
    }

private:
    Slider slider_;
    Rectangle<int> slider_position_;
    Rectangle<int> label_position_;
    SliderParameterAttachment attachment_;
    Label label_;
    Label view_label_;
};
