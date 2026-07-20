#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <random>
#include "Grain.h"

// GranularEngine: captures the incoming signal into a circular buffer and
// continuously spawns overlapping grains that read back from it. Time-stretch
// is achieved by decoupling the grain-read "scrub" speed (stretchSpeed) from
// real time, and pitch is achieved with a separate per-grain playback rate.
class GranularEngine
{
public:
    GranularEngine();

    void prepare (double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    // Pull latest parameter values once per block before calling process().
    struct Params
    {
        float stretchSpeed = 1.0f;   // 0.1x .. 4x scrub speed through buffer
        float offsetNorm   = 0.0f;   // 0..1 position offset into buffer window
        float retrigger    = 0.0f;   // 0..1 amount of phase reset per grain
        float densityHz    = 20.0f;  // grains spawned per second
        float pitchSemis   = 0.0f;   // -24..+24
        int   count        = 8;      // max simultaneous grains (voices)
        float sizeMs       = 120.0f; // grain length in ms
        float scaleAmt     = 0.0f;   // 0..1 random size variation
        GrainShape shape   = GrainShape::Hann;
        float panSpread    = 0.0f;   // 0..1 random pan spread
        bool  reverse      = false;
        float dryWet       = 0.5f;   // 0..1
    };

    void setParams (const Params& p) { params = p; }

    void process (juce::AudioBuffer<float>& buffer);

private:
    void spawnGrain();

    Params params;

    juce::AudioBuffer<float> captureBuffer;
    int   captureWritePos = 0;
    int   captureLengthSamples = 0;

    static constexpr int kMaxGrains = 32;
    std::array<Grain, kMaxGrains> grains;

    double sampleRate = 44100.0;
    double samplesUntilNextGrain = 0.0;

    std::mt19937 rng { 0xC0FFEEu };
    std::uniform_real_distribution<float> uniDist { -1.0f, 1.0f };
    std::uniform_real_distribution<float> uni01 { 0.0f, 1.0f };

    double scrubPos = 0.0; // moving read head in the capture buffer, in samples
};
