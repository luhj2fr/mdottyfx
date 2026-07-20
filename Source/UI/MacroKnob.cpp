#include "MacroKnob.h"

MacroKnob::MacroKnob (juce::AudioProcessorValueTreeState& state, const juce::String& paramID,
                      const juce::String& labelText, juce::Colour accent)
    : accentColour (accent)
{
    slider.getProperties().set ("accentColour", (juce::int64) accent.getARGB());
    slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                juce::MathConstants<float>::pi * 2.8f, true);
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, accent);
    label.setFont (juce::Font (14.0f, juce::Font::bold).withExtraKerningFactor (0.12f));
    addAndMakeVisible (label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, paramID, slider);
}

void MacroKnob::resized()
{
    auto bounds = getLocalBounds();
    label.setBounds (bounds.removeFromBottom (20));
    slider.setBounds (bounds.reduced (4));
}

void MacroKnob::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}
