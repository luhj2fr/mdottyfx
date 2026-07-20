#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyfxAudioProcessorEditor::MdottyfxAudioProcessorEditor(MdottyfxAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setLookAndFeel(&customLNF);
    addAndMakeVisible(visualizer);

    // Preset Menu Configuration
    const auto& presets = audioProcessor.getPresets();
    for (size_t i = 0; i < presets.size(); ++i) {
        presetSelector.addItem(presets[i].category + " | " + presets[i].name, (int)i + 1);
    }
    
    presetSelector.setSelectedId(audioProcessor.getCurrentProgram() + 1, juce::dontSendNotification);
    presetSelector.onChange = [this]() {
        int index = presetSelector.getSelectedId() - 1;
        audioProcessor.setCurrentProgram(index);
    };

    prevPresetBtn.onClick = [this]() {
        int newIdx = audioProcessor.getCurrentProgram() - 1;
        if (newIdx >= 0) {
            presetSelector.setSelectedId(newIdx + 1);
        }
    };

    nextPresetBtn.onClick = [this]() {
        int newIdx = audioProcessor.getCurrentProgram() + 1;
        if (newIdx < audioProcessor.getNumPrograms()) {
            presetSelector.setSelectedId(newIdx + 1);
        }
    };

    addAndMakeVisible(presetSelector);
    addAndMakeVisible(prevPresetBtn);
    addAndMakeVisible(nextPresetBtn);

    // Sliders setup
    auto setupSlider = [this](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(slider);
    };

    setupSlider(macro1Slider);
    setupSlider(macro2Slider);
    setupSlider(mixSlider);

    macro1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "MACRO1", macro1Slider);
    macro2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "MACRO2", macro2Slider);
    mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "MIX", mixSlider);

    setSize(900, 560);
}

MdottyfxAudioProcessorEditor::~MdottyfxAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void MdottyfxAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff12131a));

    // Title Branding
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    g.drawText("MDOTTYFX", 30, 18, 200, 30, juce::Justification::left);

    g.setColour(juce::Colour(0xff71717a));
    g.setFont(juce::FontOptions(12.0f, juce::Font::plain));
    g.drawText("BY PRODBYMDOTTY", 30, 46, 200, 20, juce::Justification::left);

    // Control Headers
    g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffa855f7));
    g.drawText("MACRO 1", 710, 50, 120, 20, juce::Justification::centred);
    g.drawText("MACRO 2", 710, 210, 120, 20, juce::Justification::centred);
    g.drawText("DRY / WET", 710, 370, 120, 20, juce::Justification::centred);
}

void MdottyfxAudioProcessorEditor::resized() {
    // Preset Bar (Top Center)
    prevPresetBtn.setBounds(330, 22, 30, 30);
    presetSelector.setBounds(365, 22, 230, 30);
    nextPresetBtn.setBounds(600, 22, 30, 30);

    // Central Particle Orb & Macros
    visualizer.setBounds(30, 80, 640, 440);

    macro1Slider.setBounds(710, 75, 120, 120);
    macro2Slider.setBounds(710, 235, 120, 120);
    mixSlider.setBounds(710, 395, 120, 120);
}
