#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PortalKnobLookAndFeel : public juce::LookAndFeel_V4 {
public:
    PortalKnobLookAndFeel() {
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff22222d));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override {
        auto radius = (float)juce::jmin(width, height) / 2.0f - 10.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 4.0f);

        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(juce::Colour(0xff06b6d4));
        g.strokePath(arc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path p;
        p.addRectangle(-2.0f, -radius, 4.0f, radius * 0.6f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};

class GranularParticleVisualizer : public juce::Component, private juce::Timer {
public:
    GranularParticleVisualizer() { startTimerHz(60); }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        auto centre = bounds.getCentre();

        juce::ColourGradient bgGrad(juce::Colour(0xff0d0e15), centre.x, centre.y,
                                    juce::Colour(0xff050508), 0.0f, 0.0f, true);
        g.setGradientFill(bgGrad);
        g.fillRoundedRectangle(bounds, 12.0f);

        g.setColour(juce::Colour(0x33a855f7));
        g.drawEllipse(centre.x - 120.0f, centre.y - 120.0f, 240.0f, 240.0f, 2.0f);

        for (int i = 0; i < 36; ++i) {
            float angle = i * (juce::MathConstants<float>::twoPi / 36.0f) + phase;
            float dist = 40.0f + 70.0f * std::sin(angle * 3.0f + phase * 2.0f);
            
            float x = centre.x + std::cos(angle) * dist;
            float y = centre.y + std::sin(angle) * dist;

            g.setColour(juce::Colour(0x8806b6d4));
            g.drawLine(centre.x, centre.y, x, y, 1.0f);

            g.setColour(juce::Colours::white);
            g.fillEllipse(x - 3.0f, y - 3.0f, 6.0f, 6.0f);
        }

        float coreRadius = 15.0f + 5.0f * std::sin(phase * 4.0f);
        g.setColour(juce::Colour(0xffa855f7));
        g.fillEllipse(centre.x - coreRadius, centre.y - coreRadius, coreRadius * 2.0f, coreRadius * 2.0f);
    }

private:
    void timerCallback() override {
        phase += 0.02f;
        if (phase > juce::MathConstants<float>::twoPi) phase -= juce::MathConstants<float>::twoPi;
        repaint();
    }

    float phase = 0.0f;
};

class MdottyfxAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    MdottyfxAudioProcessorEditor(MdottyfxAudioProcessor&);
    ~MdottyfxAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MdottyfxAudioProcessor& audioProcessor;
    
    PortalKnobLookAndFeel customLNF;
    GranularParticleVisualizer visualizer;

    juce::Slider macro1Slider, macro2Slider, mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> macro1Attach, macro2Attach, mixAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MdottyfxAudioProcessorEditor)
};