#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/GranularEngine.h"
#include "DSP/GrainDelay.h"

// Parameter ID strings used everywhere in the UI + DSP.
namespace ParamIDs
{
    static const juce::String macro1        = "macro1";
    static const juce::String macro2        = "macro2";
    static const juce::String xyX           = "xyX";
    static const juce::String xyY           = "xyY";
    static const juce::String reverse       = "reverse";
    static const juce::String dryWet        = "dryWet";

    static const juce::String stretchSpeed  = "stretchSpeed";
    static const juce::String stretchSync   = "stretchSync";
    static const juce::String offset        = "offset";
    static const juce::String retrigger     = "retrigger";
    static const juce::String density       = "density";
    static const juce::String densitySync   = "densitySync";
    static const juce::String pitch         = "pitch";
    static const juce::String count         = "count";
    static const juce::String size          = "size";
    static const juce::String scale         = "scale";
    static const juce::String shape         = "shape";
    static const juce::String pan           = "pan";

    static const juce::String delayTime     = "delayTime";
    static const juce::String delaySync     = "delaySync";
    static const juce::String delayFeedback = "delayFeedback";
    static const juce::String delayHP       = "delayHP";
    static const juce::String delayLP       = "delayLP";

    static const juce::String mod1Rate      = "mod1Rate";
    static const juce::String mod1Humanize  = "mod1Humanize";
    static const juce::String mod2Rate      = "mod2Rate";
    static const juce::String mod2Humanize  = "mod2Humanize";
}

class MdottyFXAudioProcessor : public juce::AudioProcessor
{
public:
    MdottyFXAudioProcessor();
    ~MdottyFXAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MDOTTYFX"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    GranularEngine granularEngine;
    GrainDelay     grainDelay;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MdottyFXAudioProcessor)
};
