#include "XYPad.h"

XYPad::XYPad (juce::AudioProcessorValueTreeState& state, const juce::String& xParamID, const juce::String& yParamID)
    : apvts (state), xID (xParamID), yID (yParamID)
{
    puckNorm.x = apvts.getRawParameterValue (xID)->load();
    puckNorm.y = apvts.getRawParameterValue (yID)->load();
    startTimerHz (30);
}

XYPad::~XYPad() { stopTimer(); }

void XYPad::timerCallback()
{
    phase += 0.01f;
    if (phase > juce::MathConstants<float>::twoPi) phase -= juce::MathConstants<float>::twoPi;
    repaint();
}

void XYPad::mouseDown (const juce::MouseEvent& e) { updateFromMouse (e.position); }
void XYPad::mouseDrag (const juce::MouseEvent& e) { updateFromMouse (e.position); }

void XYPad::updateFromMouse (juce::Point<float> localPos)
{
    auto bounds = getLocalBounds().toFloat();
    float nx = juce::jlimit (0.0f, 1.0f, localPos.x / bounds.getWidth());
    float ny = juce::jlimit (0.0f, 1.0f, localPos.y / bounds.getHeight());
    puckNorm = { nx, ny };

    if (auto* p = apvts.getParameter (xID))
    {
        p->beginChangeGesture(); p->setValueNotifyingHost (nx); p->endChangeGesture();
    }
    if (auto* p = apvts.getParameter (yID))
    {
        p->beginChangeGesture(); p->setValueNotifyingHost (ny); p->endChangeGesture();
    }
    repaint();
}

void XYPad::resized() {}

void XYPad::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto centre = bounds.getCentre();
    float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.46f;

    // Backdrop.
    g.setColour (MdottyColours::bg1);
    g.fillRect (bounds);

    // Faint static starfield speckle.
    juce::Random rnd (42);
    for (int i = 0; i < 60; ++i)
    {
        float sx = rnd.nextFloat() * bounds.getWidth();
        float sy = rnd.nextFloat() * bounds.getHeight();
        float a = 0.15f + 0.15f * rnd.nextFloat();
        g.setColour (juce::Colours::white.withAlpha (a));
        g.fillEllipse (sx, sy, 1.2f, 1.2f);
    }

    // Outer glow ring, colour blended between the two macro accents.
    juce::Colour ringColour = MdottyColours::accentPurple.interpolatedWith (MdottyColours::accentTeal, 0.5f);
    float pulse = 0.5f + 0.5f * std::sin (phase);
    g.setColour (ringColour.withAlpha (0.18f + 0.08f * pulse));
    g.drawEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 10.0f);
    g.setColour (ringColour.withAlpha (0.75f));
    g.drawEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.6f);

    // Radiating dotted rays, gently rotating.
    for (int r = 0; r < kNumRays; ++r)
    {
        float rayAngle = (juce::MathConstants<float>::twoPi * r / (float) kNumRays) + phase * 0.15f;
        for (int d = 1; d <= kDotsPerRay; ++d)
        {
            float t = (float) d / (float) (kDotsPerRay + 1);
            float dotRadius = radius * t;
            juce::Point<float> p = centre.getPointOnCircumference (dotRadius, rayAngle);
            float size = 2.2f - t * 1.2f;
            float alpha = 0.5f - t * 0.3f;
            g.setColour (juce::Colours::white.withAlpha (juce::jmax (0.08f, alpha)));
            g.fillEllipse (p.x - size * 0.5f, p.y - size * 0.5f, size, size);
        }
    }

    // Draggable puck, positioned by the two macro values, coloured by blend.
    juce::Point<float> puckPos { bounds.getX() + puckNorm.x * bounds.getWidth(),
                                  bounds.getY() + puckNorm.y * bounds.getHeight() };
    float puckR = 9.0f;
    g.setColour (MdottyColours::accentPurple.withAlpha (0.5f));
    g.drawEllipse (puckPos.x - puckR - 4.0f, puckPos.y - puckR - 4.0f, (puckR + 4.0f) * 2.0f, (puckR + 4.0f) * 2.0f, 1.5f);
    g.setColour (MdottyColours::accentTeal);
    g.drawEllipse (puckPos.x - puckR, puckPos.y - puckR, puckR * 2.0f, puckR * 2.0f, 2.0f);
    g.setColour (juce::Colours::white.withAlpha (0.9f));
    g.fillEllipse (puckPos.x - 3.0f, puckPos.y - 3.0f, 6.0f, 6.0f);
}
