#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyfxAudioProcessorEditor::MdottyfxAudioProcessorEditor(MdottyfxAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setLookAndFeel(&customLNF);

    addAndMakeVisible(visualizer);

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

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    g.drawText("MDOTTYFX", 30, 20, 200, 30, juce::Justification::left);

    g.setColour(juce::Colour(0xff71717a));
    g.setFont(juce::FontOptions(13.0f, juce::Font::plain));
    g.drawText("BY PRODBYMDOTTY", 30, 48, 200, 20, juce::Justification::left);

    g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffa855f7));
    g.drawText("MACRO 1", 710, 50, 120, 20, juce::Justification::centred);
    g.drawText("MACRO 2", 710, 210, 120, 20, juce::Justification::centred);
    g.drawText("DRY / WET", 710, 370, 120, 20, juce::Justification::centred);
}

void MdottyfxAudioProcessorEditor::resized() {
    visualizer.setBounds(30, 80, 640, 440);

    macro1Slider.setBounds(710, 75, 120, 120);
    macro2Slider.setBounds(710, 235, 120, 120);
    mixSlider.setBounds(710, 395, 120, 120);
}