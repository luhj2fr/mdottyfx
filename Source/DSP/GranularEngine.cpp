#include "GranularEngine.h"
#include <cmath>

GranularEngine::GranularEngine() {}

void GranularEngine::prepare (double newSampleRate, int /*samplesPerBlock*/, int numChannels)
{
    sampleRate = newSampleRate;

    // ~4 seconds of history to scrub / stretch through.
    captureLengthSamples = (int) (sampleRate * 4.0);
    captureBuffer.setSize (juce::jmax (2, numChannels), captureLengthSamples);
    captureBuffer.clear();

    captureWritePos = 0;
    scrubPos = 0.0;
    samplesUntilNextGrain = 0.0;

    for (auto& g : grains)
        g.active = false;
}

void GranularEngine::reset()
{
    captureBuffer.clear();
    captureWritePos = 0;
    scrubPos = 0.0;
    for (auto& g : grains)
        g.active = false;
}

void GranularEngine::spawnGrain()
{
    // Find a free voice; steal the oldest if none are free.
    int slot = -1;
    double oldestAge = -1.0;
    int oldestIdx = 0;
    for (int i = 0; i < params.count && i < kMaxGrains; ++i)
    {
        if (! grains[(size_t) i].active) { slot = i; break; }
        if (grains[(size_t) i].age > oldestAge) { oldestAge = grains[(size_t) i].age; oldestIdx = i; }
    }
    if (slot < 0) slot = oldestIdx;

    Grain& g = grains[(size_t) slot];

    double sizeMs = params.sizeMs;
    if (params.scaleAmt > 0.0f)
        sizeMs *= (1.0 + params.scaleAmt * uniDist (rng) * 0.5);
    sizeMs = juce::jlimit (5.0, 2000.0, sizeMs);

    g.lengthSamples = juce::jmax (16.0, sizeMs * 0.001 * sampleRate);
    g.age = 0.0;
    g.active = true;
    g.shape = params.shape;
    g.reverse = params.reverse;

    // Read position: current scrub head, with a little jitter based on retrigger amount.
    double jitter = params.retrigger * uni01 (rng) * g.lengthSamples * 0.5;
    g.readPos = scrubPos + jitter;

    // Wrap into a safe range behind the write head so we never read unrecorded audio.
    double maxBack = (double) captureLengthSamples - g.lengthSamples - 4.0;
    if (maxBack < 1.0) maxBack = 1.0;
    g.readPos = std::fmod (g.readPos, maxBack);
    if (g.readPos < 0) g.readPos += maxBack;

    double pitchRatio = std::pow (2.0, params.pitchSemis / 12.0);
    g.playbackRate = g.reverse ? -pitchRatio : pitchRatio;

    float pan = juce::jlimit (-1.0f, 1.0f, params.panSpread * uniDist (rng));
    float panAngle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi; // 0..pi/2
    g.panL = std::cos (panAngle);
    g.panR = std::sin (panAngle);
}

void GranularEngine::process (juce::AudioBuffer<float>& buffer)
{
    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const int capCh = captureBuffer.getNumChannels();

    // 1) Write input into the circular capture buffer.
    for (int ch = 0; ch < capCh; ++ch)
    {
        int srcCh = juce::jmin (ch, numCh - 1);
        const float* src = buffer.getReadPointer (srcCh);
        float* dst = captureBuffer.getWritePointer (ch);

        int writePos = captureWritePos;
        for (int i = 0; i < numSamples; ++i)
        {
            dst[writePos] = src[i];
            writePos = (writePos + 1) % captureLengthSamples;
        }
    }

    juce::AudioBuffer<float> wet (numCh, numSamples);
    wet.clear();

    const double offsetSamples = (double) params.offsetNorm * captureLengthSamples * 0.5;
    const double grainIntervalSamples = sampleRate / juce::jmax (0.5f, params.densityHz);

    for (int i = 0; i < numSamples; ++i)
    {
        // Advance the scrub head. stretchSpeed of 1.0 == moves at real-time rate.
        double dir = params.reverse ? -1.0 : 1.0;
        scrubPos += dir * params.stretchSpeed;

        double windowLen = (double) captureLengthSamples * 0.5;
        double refPos = (double) captureWritePos - offsetSamples;
        while (refPos < 0) refPos += captureLengthSamples;

        // Keep scrubPos wrapping within a window behind the reference point.
        if (scrubPos > windowLen) scrubPos -= windowLen;
        if (scrubPos < 0.0) scrubPos += windowLen;

        // Spawn new grains at the configured density.
        samplesUntilNextGrain -= 1.0;
        if (samplesUntilNextGrain <= 0.0)
        {
            samplesUntilNextGrain += grainIntervalSamples;
            spawnGrain();
        }

        // Render all active grains into this sample.
        for (auto& g : grains)
        {
            if (! g.active) continue;

            float gain = g.windowGain();

            for (int ch = 0; ch < numCh; ++ch)
            {
                int capChIdx = juce::jmin (ch, capCh - 1);
                const float* capData = captureBuffer.getReadPointer (capChIdx);

                double pos = g.readPos;
                int idx0 = ((int) pos) % captureLengthSamples;
                if (idx0 < 0) idx0 += captureLengthSamples;
                int idx1 = (idx0 + 1) % captureLengthSamples;
                float frac = (float) (pos - std::floor (pos));

                float s = capData[idx0] + frac * (capData[idx1] - capData[idx0]);
                float panGain = (ch == 0) ? g.panL : g.panR;

                wet.getWritePointer (ch)[i] += s * gain * panGain;
            }

            g.readPos += g.playbackRate;
            double maxBack = (double) captureLengthSamples - g.lengthSamples - 4.0;
            if (maxBack < 1.0) maxBack = 1.0;
            if (g.readPos < 0) g.readPos += maxBack;
            if (g.readPos > maxBack) g.readPos -= maxBack;

            g.age += 1.0;
            if (g.age >= g.lengthSamples)
                g.active = false;
        }
    }

    captureWritePos = (captureWritePos + numSamples) % captureLengthSamples;

    // Dry/wet mix back into the host buffer.
    const float wetAmt = params.dryWet;
    const float dryAmt = 1.0f - wetAmt;
    for (int ch = 0; ch < numCh; ++ch)
    {
        float* out = buffer.getWritePointer (ch);
        const float* w = wet.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            out[i] = out[i] * dryAmt + w[i] * wetAmt;
    }
}
