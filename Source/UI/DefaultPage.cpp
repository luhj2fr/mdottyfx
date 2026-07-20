#include "DefaultPage.h"
#include "MdottyLookAndFeel.h"
#include "../PluginProcessor.h"

DefaultPage::DefaultPage (juce::AudioProcessorValueTreeState& state)
    : xyPad (state, ParamIDs::xyX, ParamIDs::xyY)
{
    titleLabel.setText ("M D O T T Y F X", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setFont (juce::Font (26.0f, juce::Font::bold).withExtraKerningFactor (0.25f));
    titleLabel.setColour (juce::Label::textColourId, MdottyColours::textBright);
    addAndMakeVisible (titleLabel);

    addAndMakeVisible (xyPad);
}

void DefaultPage::resized()
{
    auto bounds = getLocalBounds().reduced (24);
    titleLabel.setBounds (bounds.removeFromTop (40));
    bounds.removeFromTop (10);
    xyPad.setBounds (bounds);
}

void DefaultPage::paint (juce::Graphics& g)
{
    g.setColour (MdottyColours::bg1);
    g.fillRect (getLocalBounds());
}
