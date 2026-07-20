#include "GrainControlsPage.h"
#include "MdottyLookAndFeel.h"
#include "../PluginProcessor.h"

std::unique_ptr<MacroKnob> GrainControlsPage::makeKnob (const juce::String& id, const juce::String& label, juce::Colour c)
{
    return std::make_unique<MacroKnob> (apvts, id, label, c);
}

GrainControlsPage::GrainControlsPage (juce::AudioProcessorValueTreeState& state)
    : apvts (state), miniXY (state, ParamIDs::xyX, ParamIDs::xyY)
{
    auto neutral = MdottyColours::textDim;

    sectionTitle.setText ("GRAIN CONTROLS", juce::dontSendNotification);
    sectionTitle.setFont (juce::Font (16.0f, juce::Font::bold).withExtraKerningFactor (0.2f));
    sectionTitle.setColour (juce::Label::textColourId, MdottyColours::textBright);
    addAndMakeVisible (sectionTitle);

    delaySectionTitle.setText ("GRAIN DELAY", juce::dontSendNotification);
    delaySectionTitle.setFont (juce::Font (14.0f, juce::Font::bold).withExtraKerningFactor (0.2f));
    delaySectionTitle.setColour (juce::Label::textColourId, MdottyColours::textDim);
    addAndMakeVisible (delaySectionTitle);

    xyLabel.setText ("XY CONTROL", juce::dontSendNotification);
    xyLabel.setFont (juce::Font (12.0f, juce::Font::bold).withExtraKerningFactor (0.2f));
    xyLabel.setColour (juce::Label::textColourId, MdottyColours::textDim);
    addAndMakeVisible (xyLabel);

    stretchSpeed = makeKnob (ParamIDs::stretchSpeed, "SPEED", MdottyColours::accentPurple);
    offset       = makeKnob (ParamIDs::offset,       "OFFSET", neutral);
    retrigger    = makeKnob (ParamIDs::retrigger,    "RETRIGGER", neutral);
    density      = makeKnob (ParamIDs::density,      "DENSITY", MdottyColours::accentTeal);
    pitch        = makeKnob (ParamIDs::pitch,        "PITCH", neutral);
    count        = makeKnob (ParamIDs::count,        "COUNT", neutral);
    size         = makeKnob (ParamIDs::size,         "SIZE", MdottyColours::accentPurple);
    scale        = makeKnob (ParamIDs::scale,        "SCALE", neutral);
    pan          = makeKnob (ParamIDs::pan,          "PAN", neutral);

    for (auto* k : { stretchSpeed.get(), offset.get(), retrigger.get(), density.get(), pitch.get(),
                     count.get(), size.get(), scale.get(), pan.get() })
        addAndMakeVisible (*k);

    delayTime     = makeKnob (ParamIDs::delayTime, "TIME", neutral);
    delayFeedback = makeKnob (ParamIDs::delayFeedback, "FEEDBACK", neutral);
    delayHP       = makeKnob (ParamIDs::delayHP, "HP", neutral);
    delayLP       = makeKnob (ParamIDs::delayLP, "LP", neutral);
    for (auto* k : { delayTime.get(), delayFeedback.get(), delayHP.get(), delayLP.get() })
        addAndMakeVisible (*k);

    mod1Rate  = makeKnob (ParamIDs::mod1Rate, "RATE", MdottyColours::accentTeal.withRotatedHue (-0.05f));
    mod1Human = makeKnob (ParamIDs::mod1Humanize, "HUMANIZE", neutral);
    mod2Rate  = makeKnob (ParamIDs::mod2Rate, "RATE", MdottyColours::accentPurple.withRotatedHue (0.05f));
    mod2Human = makeKnob (ParamIDs::mod2Humanize, "HUMANIZE", neutral);
    for (auto* k : { mod1Rate.get(), mod1Human.get(), mod2Rate.get(), mod2Human.get() })
        addAndMakeVisible (*k);

    stretchSyncAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::stretchSync, stretchSyncBtn);
    densitySyncAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::densitySync, densitySyncBtn);
    delaySyncAtt   = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::delaySync, delaySyncBtn);
    addAndMakeVisible (stretchSyncBtn);
    addAndMakeVisible (densitySyncBtn);
    addAndMakeVisible (delaySyncBtn);

    addAndMakeVisible (miniXY);
}

void GrainControlsPage::paint (juce::Graphics& g)
{
    g.setColour (MdottyColours::bg1);
    g.fillRect (getLocalBounds());
}

void GrainControlsPage::resized()
{
    auto bounds = getLocalBounds().reduced (20);

    sectionTitle.setBounds (bounds.removeFromTop (24));
    bounds.removeFromTop (8);

    auto topRow = bounds.removeFromTop (150);
    int knobW = topRow.getWidth() / 9;
    auto place = [&] (juce::Component& c, juce::Rectangle<int>& row)
    {
        c.setBounds (row.removeFromLeft (knobW).reduced (4));
    };
    place (*stretchSpeed, topRow); place (*offset, topRow); place (*retrigger, topRow);
    place (*density, topRow); place (*pitch, topRow); place (*count, topRow);
    place (*size, topRow); place (*scale, topRow); place (*pan, topRow);

    stretchSyncBtn.setBounds (stretchSpeed->getBounds().withY (stretchSpeed->getBottom() - 4).withHeight (18));
    densitySyncBtn.setBounds (density->getBounds().withY (density->getBottom() - 4).withHeight (18));

    bounds.removeFromTop (10);

    auto lowerRow = bounds.removeFromTop (150);
    auto delayArea = lowerRow.removeFromLeft (lowerRow.getWidth() * 2 / 3);
    delaySectionTitle.setBounds (delayArea.removeFromTop (18));
    int dKnobW = delayArea.getWidth() / 4;
    auto placeDelay = [&] (juce::Component& c) { c.setBounds (delayArea.removeFromLeft (dKnobW).reduced (4)); };
    placeDelay (*delayTime); placeDelay (*delayFeedback); placeDelay (*delayHP); placeDelay (*delayLP);
    delaySyncBtn.setBounds (delayTime->getBounds().withY (delayTime->getBottom() - 4).withHeight (18));

    lowerRow.removeFromLeft (10);
    xyLabel.setBounds (lowerRow.removeFromTop (18));
    miniXY.setBounds (lowerRow);

    bounds.removeFromTop (10);
    auto modRow = bounds.removeFromTop (bounds.getHeight());
    int modKnobW = modRow.getWidth() / 4;
    auto placeMod = [&] (juce::Component& c) { c.setBounds (modRow.removeFromLeft (modKnobW).reduced (6)); };
    placeMod (*mod1Rate); placeMod (*mod1Human); placeMod (*mod2Rate); placeMod (*mod2Human);
}
