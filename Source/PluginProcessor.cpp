#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyfxAudioProcessor::MdottyfxAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

MdottyfxAudioProcessor::~MdottyfxAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MdottyfxAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("MACRO1", "Macro 1", 0.01f, 0.5f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MACRO2", "Macro 2", 0.01f, 0.2f, 0.05f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Dry/Wet Mix", 0.0f, 1.0f, 0.5f));

    return { params.begin(), params.end() };
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
    copyXmlToBinary(*xml, destData);
}

void MdottyfxAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MdottyfxAudioProcessor();
}