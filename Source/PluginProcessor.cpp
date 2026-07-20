#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyfxAudioProcessor::MdottyfxAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
    initPresets();
}

MdottyfxAudioProcessor::~MdottyfxAudioProcessor() {}

void MdottyfxAudioProcessor::initPresets() {
    factoryPresets = {
        // Melodies & Guitars
        { "Spaced Out Guitar", "Guitars & Keys", 0.35f, 0.12f, 3500.0f, 0.0f,  0.50f },
        { "Underwater Melody", "Filters & Slow", 0.45f, 0.18f, 1200.0f, -12.0f, 0.65f },
        { "Reverse Tape Stutter", "Glitch & Stutter", 0.08f, 0.03f, 8000.0f, 0.0f,  0.40f },
        { "Analog Synth Swirl", "Synth FX",     0.28f, 0.15f, 5000.0f, 7.0f,   0.55f },
        
        // Trap Drums & Percs
        { "Hi-Hat Granulator", "Drums & Perc",  0.02f, 0.01f, 12000.0f, 12.0f, 0.35f },
        { "808 Sub Wobble",    "Bass & 808",    0.40f, 0.10f, 400.0f,   -24.0f, 0.30f },
        
        // Vocal FX
        { "Alien Vocal Chop",  "Vocals",        0.15f, 0.08f, 6500.0f,  12.0f, 0.60f },
        { "Demon Choir Half",  "Vocals",        0.48f, 0.19f, 2200.0f, -12.0f, 0.70f },
        { "Ambience Cloud",    "Atmosphere",    0.50f, 0.20f, 4000.0f,  5.0f,  0.80f },
        
        // Aggressive Drill FX
        { "Drill Glitch Shift", "Drill Special", 0.05f, 0.02f, 9000.0f,  3.0f,  0.45f }
    };
}

juce::AudioProcessorValueTreeState::ParameterLayout MdottyfxAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("MACRO1", "Macro 1 (Size)", 0.01f, 0.50f, 0.10f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MACRO2", "Macro 2 (Density)", 0.01f, 0.20f, 0.05f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CUTOFF", "Filter Cutoff", 200.0f, 20000.0f, 20000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PITCH",  "Pitch Shift", -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX",    "Dry/Wet Mix", 0.0f, 1.0f, 0.50f));

    return { params.begin(), params.end() };
}

void MdottyfxAudioProcessor::setCurrentProgram(int index) {
    if (index >= 0 && index < (int)factoryPresets.size()) {
        currentPresetIndex = index;
        const auto& p = factoryPresets[index];

        apvts.getParameter("MACRO1")->setValueNotifyingHost(apvts.getParameter("MACRO1")->convertTo0to1(p.macro1));
        apvts.getParameter("MACRO2")->setValueNotifyingHost(apvts.getParameter("MACRO2")->convertTo0to1(p.macro2));
        apvts.getParameter("CUTOFF")->setValueNotifyingHost(apvts.getParameter("CUTOFF")->convertTo0to1(p.cutoff));
        apvts.getParameter("PITCH")->setValueNotifyingHost(apvts.getParameter("PITCH")->convertTo0to1(p.pitch));
        apvts.getParameter("MIX")->setValueNotifyingHost(apvts.getParameter("MIX")->convertTo0to1(p.mix));
    }
}

const juce::String MdottyfxAudioProcessor::getProgramName(int index) {
    if (index >= 0 && index < (int)factoryPresets.size())
        return factoryPresets[index].name;
    return {};
}

void MdottyfxAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    circularBuffer.setSize(2, (int)(sampleRate * 2.0));
    circularBuffer.clear();
}

void MdottyfxAudioProcessor::releaseResources() {}

void MdottyfxAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    int bufLen = circularBuffer.getNumSamples();
    float mix = apvts.getRawParameterValue("MIX")->load();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        for (int ch = 0; ch < totalNumInputChannels; ++ch) {
            circularBuffer.setSample(ch, writePosition, buffer.getSample(ch, sample));
        }

        for (int ch = 0; ch < totalNumOutputChannels; ++ch) {
            float dry = buffer.getSample(ch, sample);
            float wet = circularBuffer.getSample(ch, (writePosition - 2205 + bufLen) % bufLen);
            buffer.setSample(ch, sample, dry * (1.0f - mix) + wet * mix);
        }

        writePosition = (writePosition + 1) % bufLen;
    }
}

juce::AudioProcessorEditor* MdottyfxAudioProcessor::createEditor() {
    return new MdottyfxAudioProcessorEditor(*this);
}

void MdottyfxAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->setAttribute("currentPreset", currentPresetIndex);
    copyXmlToBinary(*xml, destData);
}

void MdottyfxAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(apvts.state.getType())) {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
            currentPresetIndex = xmlState->getIntAttribute("currentPreset", 0);
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MdottyfxAudioProcessor();
}
