#pragma once
#include "PluginProcessor.h"
#include "UI/MdottyLookAndFeel.h"
#include "UI/MacroKnob.h"
#include "UI/DefaultPage.h"
#include "UI/GrainControlsPage.h"
#include "UI/PresetBrowser.h"

class MdottyFXAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MdottyFXAudioProcessorEditor (MdottyFXAudioProcessor&);
    ~MdottyFXAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void showPage (int index); // 0 = default/XY, 1 = grain controls
    void showPresetBrowser (bool show);

    MdottyFXAudioProcessor& processor;
    MdottyLookAndFeel lookAndFeel;

    // Top bar
    juce::Label logoLabel;
    juce::TextButton presetButton { juce::CharPointer_UTF8 ("\u25A6") };

    // Left icon rail
    juce::TextButton defaultPageBtn { juce::CharPointer_UTF8 ("\u25CB") };
    juce::TextButton grainPageBtn   { juce::CharPointer_UTF8 ("\u2261") };

    // Pages (main content area)
    DefaultPage defaultPage;
    GrainControlsPage grainPage;
    PresetBrowser presetBrowser;
    int currentPage = 0;

    // Right sidebar
    juce::Label sidebarTitle;
    MacroKnob macro1Knob, macro2Knob;
    juce::ToggleButton reverseToggle { "REVERSE" };
    juce::Slider dryWetSlider { juce::Slider::LinearHorizontal, juce::Slider::NoTextBox };
    juce::Label dryLabel, wetLabel, brandLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MdottyFXAudioProcessorEditor)
};
