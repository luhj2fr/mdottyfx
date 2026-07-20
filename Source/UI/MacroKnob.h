#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "MdottyLookAndFeel.h"

// A big labeled rotary macro knob (MACRO 1 / MACRO 2 style) with an accent colour.
class MacroKnob : public juce::Component
{
public:
    MacroKnob (juce::AudioProcessorValueTreeState& state, const juce::String& paramID,
               const juce::String& labelText, juce::Colour accent);

    void resized() override;
    void paint (juce::Graphics&) override;

private:
    juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    juce::Label label;
    juce::Colour accentColour;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};
