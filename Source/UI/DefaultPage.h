#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "XYPad.h"

class DefaultPage : public juce::Component
{
public:
    explicit DefaultPage (juce::AudioProcessorValueTreeState& state);
    void resized() override;
    void paint (juce::Graphics&) override;

private:
    juce::Label titleLabel;
    XYPad xyPad;
};
