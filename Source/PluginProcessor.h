#pragma once
#include <JuceHeader.h>

struct Preset {
    juce::String name;
    juce::String category;
    float macro1;   // Grain Size
    float macro2;   // Density
    float cutoff;   // Low Pass Filter Cutoff
    float pitch;    // Pitch Shift
    float mix;      // Dry/Wet Mix
};

class MdottyfxAudioProcessor : public juce::AudioProcessor {
public:
    MdottyfxAudioProcessor();
    ~MdottyfxAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return (int)factoryPresets.size(); }
    int getCurrentProgram() override { return currentPresetIndex; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    const std::vector<Preset>& getPresets() const { return factoryPresets; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void initPresets();

    std::vector<Preset> factoryPresets;
    int currentPresetIndex = 0;

    juce::AudioBuffer<float> circularBuffer;
    int writePosition = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MdottyfxAudioProcessor)
};
