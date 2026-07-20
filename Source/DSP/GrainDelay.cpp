#include "GrainDelay.h"

void GrainDelay::prepare (double newSampleRate, int samplesPerBlock, int numChannels)
{
    sampleRate = newSampleRate;
    numChannelsPrepped = numChannels;

    juce::dsp::ProcessSpec spec { newSampleRate, (juce::uint32) samplesPerBlock, (juce::uint32) numChannels };
    delayLine.prepare (spec);
    delayLine.setMaximumDelayInSamples (1 << 18);

    hpFilters.resize ((size_t) numChannels);
    lpFilters.resize ((size_t) numChannels);
    for (auto& f : hpFilters) f.prepare (spec);
    for (auto& f : lpFilters) f.prepare (spec);
    reset();
}

void GrainDelay::reset()
{
    delayLine.reset();
    for (auto& f : hpFilters) f.reset();
    for (auto& f : lpFilters) f.reset();
}

void GrainDelay::process (juce::AudioBuffer<float>& buffer)
{
    const int numCh = juce::jmin (buffer.getNumChannels(), numChannelsPrepped);
    const int numSamples = buffer.getNumSamples();

    double delaySamples = juce::jlimit (1.0, (double) (1 << 18) - 8.0,
                                         (double) params.timeMs * 0.001 * sampleRate);

    for (int ch = 0; ch < numCh; ++ch)
    {
        auto coeffsHP = juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, juce::jlimit (20.0f, 20000.0f, params.hpHz));
        auto coeffsLP = juce::dsp::IIR::Coefficients<float>::makeLowPass (sampleRate, juce::jlimit (20.0f, 20000.0f, params.lpHz));
        *hpFilters[(size_t) ch].coefficients = *coeffsHP;
        *lpFilters[(size_t) ch].coefficients = *coeffsLP;

        float* data = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
        {
            float input = data[i];

            float delayed = delayLine.popSample (ch, (float) delaySamples, false);
            float filtered = hpFilters[(size_t) ch].processSample (delayed);
            filtered = lpFilters[(size_t) ch].processSample (filtered);

            float toPush = input + filtered * juce::jlimit (0.0f, 0.95f, params.feedback);
            delayLine.pushSample (ch, toPush);

            data[i] = input * (1.0f - params.mix) + filtered * params.mix;
        }
    }
}
