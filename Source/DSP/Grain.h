#pragma once
#include <cmath>
#include <juce_core/juce_core.h>

enum class GrainShape
{
    Hann = 0,
    Triangle,
    Gaussian,
    Trapezoid,
    NumShapes
};

// A single grain reading from the circular capture buffer.
struct Grain
{
    bool   active        = false;
    double readPos        = 0.0;   // position in the capture buffer, in samples
    double playbackRate   = 1.0;   // combines pitch ratio
    double lengthSamples   = 0.0;  // total grain length in samples
    double age             = 0.0;  // samples elapsed since spawn
    float  panL             = 0.707f;
    float  panR             = 0.707f;
    bool   reverse          = false;
    GrainShape shape        = GrainShape::Hann;

    // Returns the window gain [0..1] for the grain's current age.
    float windowGain() const
    {
        if (lengthSamples <= 0.0) return 0.0f;
        double t = juce::jlimit (0.0, 1.0, age / lengthSamples);

        switch (shape)
        {
            case GrainShape::Hann:
                return (float) (0.5 - 0.5 * std::cos (2.0 * juce::MathConstants<double>::pi * t));
            case GrainShape::Triangle:
                return (float) (1.0 - std::abs (2.0 * t - 1.0));
            case GrainShape::Gaussian:
            {
                double x = (t - 0.5) / 0.2;
                return (float) std::exp (-0.5 * x * x);
            }
            case GrainShape::Trapezoid:
            {
                double edge = 0.15;
                if (t < edge)  return (float) (t / edge);
                if (t > 1.0 - edge) return (float) ((1.0 - t) / edge);
                return 1.0f;
            }
            default:
                return 1.0f;
        }
    }
};
