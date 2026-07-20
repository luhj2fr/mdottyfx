#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "MdottyLookAndFeel.h"

// Animated circular XY control: radiating dot-lines from centre (like Portal's
// hero visual), a pulsing outer ring in the two macro accent colours, and a
// draggable puck bound to two parameters (xyX / xyY).
class XYPad : public juce::Component, private juce::Timer
{
public:
    XYPad (juce::AudioProcessorValueTreeState& state, const juce::String& xParamID, const juce::String& yParamID);
    ~XYPad() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;

private:
    void timerCallback() override;
    void updateFromMouse (juce::Point<float> localPos);

    juce::AudioProcessorValueTreeState& apvts;
    juce::String xID, yID;

    float phase = 0.0f;
    juce::Point<float> puckNorm { 0.5f, 0.5f };

    static constexpr int kNumRays = 32;
    static constexpr int kDotsPerRay = 4;
};
