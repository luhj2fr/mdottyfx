#include "PluginEditor.h"

MdottyFXAudioProcessorEditor::MdottyFXAudioProcessorEditor (MdottyFXAudioProcessor& p)
    : juce::AudioProcessorEditor (&p), processor (p),
      defaultPage (p.apvts), grainPage (p.apvts), presetBrowser (p.apvts),
      macro1Knob (p.apvts, ParamIDs::macro1, "MACRO 1", MdottyColours::accentPurple),
      macro2Knob (p.apvts, ParamIDs::macro2, "MACRO 2", MdottyColours::accentTeal)
{
    setLookAndFeel (&lookAndFeel);
    setSize (1000, 620);
    setResizable (true, true);
    setResizeLimits (760, 460, 1600, 1000);

    logoLabel.setText ("MDOTTYFX", juce::dontSendNotification);
    logoLabel.setFont (juce::Font (16.0f, juce::Font::bold).withExtraKerningFactor (0.15f));
    logoLabel.setColour (juce::Label::textColourId, MdottyColours::textBright);
    addAndMakeVisible (logoLabel);

    presetButton.onClick = [this] { showPresetBrowser (true); };
    addAndMakeVisible (presetButton);

    defaultPageBtn.onClick = [this] { showPage (0); };
    grainPageBtn.onClick   = [this] { showPage (1); };
    addAndMakeVisible (defaultPageBtn);
    addAndMakeVisible (grainPageBtn);

    addAndMakeVisible (defaultPage);
    addChildComponent (grainPage);
    addChildComponent (presetBrowser);
    presetBrowser.onClose = [this] { showPresetBrowser (false); };

    sidebarTitle.setText ("DEFAULT", juce::dontSendNotification);
    sidebarTitle.setFont (juce::Font (12.0f, juce::Font::bold).withExtraKerningFactor (0.2f));
    sidebarTitle.setColour (juce::Label::textColourId, MdottyColours::textDim);
    addAndMakeVisible (sidebarTitle);

    addAndMakeVisible (macro1Knob);
    addAndMakeVisible (macro2Knob);

    reverseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (p.apvts, ParamIDs::reverse, reverseToggle);
    addAndMakeVisible (reverseToggle);

    dryWetSlider.getProperties().set ("accentColour", (juce::int64) MdottyColours::accentTeal.getARGB());
    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (p.apvts, ParamIDs::dryWet, dryWetSlider);
    addAndMakeVisible (dryWetSlider);

    dryLabel.setText ("DRY", juce::dontSendNotification);
    wetLabel.setText ("WET", juce::dontSendNotification);
    for (auto* l : { &dryLabel, &wetLabel })
    {
        l->setFont (juce::Font (11.0f));
        l->setColour (juce::Label::textColourId, MdottyColours::textDim);
        addAndMakeVisible (*l);
    }
    dryLabel.setJustificationType (juce::Justification::centredLeft);
    wetLabel.setJustificationType (juce::Justification::centredRight);

    brandLabel.setText ("MDOTTYFX  by  ProdByMdotty", juce::dontSendNotification);
    brandLabel.setFont (juce::Font (11.0f).withExtraKerningFactor (0.1f));
    brandLabel.setColour (juce::Label::textColourId, MdottyColours::textDim.withAlpha (0.7f));
    brandLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (brandLabel);

    showPage (0);
}

MdottyFXAudioProcessorEditor::~MdottyFXAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void MdottyFXAudioProcessorEditor::showPage (int index)
{
    currentPage = index;
    defaultPage.setVisible (index == 0);
    grainPage.setVisible (index == 1);
}

void MdottyFXAudioProcessorEditor::showPresetBrowser (bool show)
{
    presetBrowser.setVisible (show);
    if (show) presetBrowser.toFront (true);
}

void MdottyFXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (MdottyColours::bg0);

    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient bgGrad (MdottyColours::bg0, bounds.getX(), bounds.getY(),
                                 MdottyColours::bg1, bounds.getRight(), bounds.getBottom(), false);
    g.setGradientFill (bgGrad);
    g.fillRect (bounds);

    // Divider between main content and sidebar.
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.drawLine ((float) (getWidth() - 210), 0.0f, (float) (getWidth() - 210), (float) getHeight(), 1.0f);
}

void MdottyFXAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto topBar = bounds.removeFromTop (36);
    logoLabel.setBounds (topBar.removeFromLeft (200).reduced (8, 4));
    presetButton.setBounds (topBar.removeFromRight (36).reduced (4));

    auto sidebar = bounds.removeFromRight (210);
    auto iconRail = bounds.removeFromRight (34);

    defaultPageBtn.setBounds (iconRail.getX(), 10, 34, 34);
    grainPageBtn.setBounds (iconRail.getX(), 50, 34, 34);

    defaultPage.setBounds (bounds);
    grainPage.setBounds (bounds);
    presetBrowser.setBounds (bounds.getUnion (iconRail));

    // Sidebar layout.
    auto sb = sidebar.reduced (16);
    sidebarTitle.setBounds (sb.removeFromTop (18));
    sb.removeFromTop (10);
    macro1Knob.setBounds (sb.removeFromTop (140));
    sb.removeFromTop (10);
    macro2Knob.setBounds (sb.removeFromTop (140));
    sb.removeFromTop (12);
    reverseToggle.setBounds (sb.removeFromTop (26));
    sb.removeFromTop (14);

    auto dwLabels = sb.removeFromTop (16);
    dryLabel.setBounds (dwLabels.removeFromLeft (dwLabels.getWidth() / 2));
    wetLabel.setBounds (dwLabels);
    dryWetSlider.setBounds (sb.removeFromTop (24));

    brandLabel.setBounds (sidebar.removeFromBottom (20).reduced (4, 0));
}
