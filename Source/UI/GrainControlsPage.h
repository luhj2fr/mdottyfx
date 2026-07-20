#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "MacroKnob.h"
#include "XYPad.h"

class GrainControlsPage : public juce::Component
{
public:
    explicit GrainControlsPage (juce::AudioProcessorValueTreeState& state);
    void resized() override;
    void paint (juce::Graphics&) override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::Label sectionTitle, delaySectionTitle, xyLabel;

    std::unique_ptr<MacroKnob> stretchSpeed, offset, retrigger, density, pitch, count, size, scale, pan;
    std::unique_ptr<MacroKnob> delayTime, delayFeedback, delayHP, delayLP;
    std::unique_ptr<MacroKnob> mod1Rate, mod1Human, mod2Rate, mod2Human;

    juce::ToggleButton stretchSyncBtn { "SYNC" }, densitySyncBtn { "SYNC" }, delaySyncBtn { "SYNC" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> stretchSyncAtt, densitySyncAtt, delaySyncAtt;

    XYPad miniXY;

    std::unique_ptr<MacroKnob> makeKnob (const juce::String& id, const juce::String& label, juce::Colour c);
};
