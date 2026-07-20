#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyFXAudioProcessor::MdottyFXAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
}

MdottyFXAudioProcessor::~MdottyFXAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MdottyFXAudioProcessor::createLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    auto pct = NormalisableRange<float> (0.0f, 1.0f);

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::macro1, "Macro 1", pct, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::macro2, "Macro 2", pct, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::xyX,    "XY X",    pct, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::xyY,    "XY Y",    pct, 0.5f));
    params.push_back (std::make_unique<AudioParameterBool>  (ParamIDs::reverse, "Reverse", false));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::dryWet, "Dry/Wet", pct, 0.5f));

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::stretchSpeed, "Stretch Speed",
                       NormalisableRange<float> (0.1f, 4.0f, 0.0f, 0.4f), 1.0f));
    params.push_back (std::make_unique<AudioParameterBool>  (ParamIDs::stretchSync, "Stretch Sync", false));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::offset, "Offset", pct, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::retrigger, "Retrigger", pct, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::density, "Density",
                       NormalisableRange<float> (1.0f, 80.0f, 0.0f, 0.5f), 20.0f));
    params.push_back (std::make_unique<AudioParameterBool>  (ParamIDs::densitySync, "Density Sync", false));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::pitch, "Pitch",
                       NormalisableRange<float> (-24.0f, 24.0f), 0.0f));
    params.push_back (std::make_unique<AudioParameterInt>   (ParamIDs::count, "Count", 1, 32, 8));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::size, "Size",
                       NormalisableRange<float> (5.0f, 2000.0f, 0.0f, 0.35f), 120.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::scale, "Scale", pct, 0.0f));
    params.push_back (std::make_unique<AudioParameterInt>   (ParamIDs::shape, "Shape", 0, 3, 0));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::pan, "Pan Spread", pct, 0.0f));

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::delayTime, "Delay Time",
                       NormalisableRange<float> (1.0f, 4000.0f, 0.0f, 0.35f), 350.0f));
    params.push_back (std::make_unique<AudioParameterBool>  (ParamIDs::delaySync, "Delay Sync", false));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::delayFeedback, "Delay Feedback", pct, 0.3f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::delayHP, "Delay HP",
                       NormalisableRange<float> (20.0f, 5000.0f, 0.0f, 0.35f), 80.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::delayLP, "Delay LP",
                       NormalisableRange<float> (200.0f, 20000.0f, 0.0f, 0.35f), 12000.0f));

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::mod1Rate, "Mod 1 Rate",
                       NormalisableRange<float> (0.01f, 20.0f, 0.0f, 0.35f), 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::mod1Humanize, "Mod 1 Humanize", pct, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::mod2Rate, "Mod 2 Rate",
                       NormalisableRange<float> (0.01f, 20.0f, 0.0f, 0.35f), 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::mod2Humanize, "Mod 2 Humanize", pct, 0.0f));

    return { params.begin(), params.end() };
}

void MdottyFXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    granularEngine.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    grainDelay.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void MdottyFXAudioProcessor::releaseResources() {}

bool MdottyFXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;
    return layouts.getMainInputChannelSet() == mainOut;
}

void MdottyFXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    GranularEngine::Params gp;
    gp.stretchSpeed = apvts.getRawParameterValue (ParamIDs::stretchSpeed)->load();
    gp.offsetNorm   = apvts.getRawParameterValue (ParamIDs::offset)->load();
    gp.retrigger    = apvts.getRawParameterValue (ParamIDs::retrigger)->load();
    gp.densityHz    = apvts.getRawParameterValue (ParamIDs::density)->load();
    gp.pitchSemis   = apvts.getRawParameterValue (ParamIDs::pitch)->load();
    gp.count        = (int) apvts.getRawParameterValue (ParamIDs::count)->load();
    gp.sizeMs       = apvts.getRawParameterValue (ParamIDs::size)->load();
    gp.scaleAmt     = apvts.getRawParameterValue (ParamIDs::scale)->load();
    gp.shape        = (GrainShape) (int) apvts.getRawParameterValue (ParamIDs::shape)->load();
    gp.panSpread    = apvts.getRawParameterValue (ParamIDs::pan)->load();
    gp.reverse      = apvts.getRawParameterValue (ParamIDs::reverse)->load() > 0.5f;
    gp.dryWet       = apvts.getRawParameterValue (ParamIDs::dryWet)->load();
    granularEngine.setParams (gp);
    granularEngine.process (buffer);

    GrainDelay::Params dp;
    dp.timeMs   = apvts.getRawParameterValue (ParamIDs::delayTime)->load();
    dp.feedback = apvts.getRawParameterValue (ParamIDs::delayFeedback)->load();
    dp.hpHz     = apvts.getRawParameterValue (ParamIDs::delayHP)->load();
    dp.lpHz     = apvts.getRawParameterValue (ParamIDs::delayLP)->load();
    dp.mix      = 0.0f; // grain delay send stays off unless user raises feedback in UI wiring
    grainDelay.setParams (dp);
    grainDelay.process (buffer);
}

juce::AudioProcessorEditor* MdottyFXAudioProcessor::createEditor()
{
    return new MdottyFXAudioProcessorEditor (*this);
}

void MdottyFXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MdottyFXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MdottyFXAudioProcessor();
}
