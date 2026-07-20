#pragma once
#include <juce_dsp/juce_dsp.h>

// Simple stereo feedback delay with HP/LP tone shaping in the feedback path,
// used post-granular (matches Portal's "Grain Delay" section).
class GrainDelay
{
public:
    void prepare (double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    struct Params
    {
        float timeMs      = 350.0f;
        float feedback    = 0.3f;   // 0..0.95
        float hpHz        = 80.0f;
        float lpHz        = 12000.0f;
        float mix         = 0.0f;   // 0 = bypass, this stage is additive/send-style
    };

    void setParams (const Params& p) { params = p; }
    void process (juce::AudioBuffer<float>& buffer);

private:
    Params params;
    double sampleRate = 44100.0;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 1 << 18 };
    std::vector<juce::dsp::IIR::Filter<float>> hpFilters, lpFilters;
    int numChannelsPrepped = 2;
};
