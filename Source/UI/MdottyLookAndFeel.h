#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace MdottyColours
{
    static const juce::Colour bg0        { 0xff0a0a0f };
    static const juce::Colour bg1        { 0xff121218 };
    static const juce::Colour panel      { 0xff17171f };
    static const juce::Colour accentPurple { 0xff8b6bff };
    static const juce::Colour accentTeal   { 0xff3ce6c8 };
    static const juce::Colour textDim    { 0xff8a8a96 };
    static const juce::Colour textBright { 0xffe8e8f0 };
    static const juce::Colour knobBody   { 0xff222229 };
    static const juce::Colour trackDim   { 0xff2a2a33 };
}

class MdottyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MdottyLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getLabelFont (juce::Label&) override;
};
