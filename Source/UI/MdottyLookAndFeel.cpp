#include "MdottyLookAndFeel.h"

MdottyLookAndFeel::MdottyLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, MdottyColours::bg0);
    setColour (juce::Slider::textBoxTextColourId, MdottyColours::textBright);
    setColour (juce::Label::textColourId, MdottyColours::textDim);
}

juce::Font MdottyLookAndFeel::getLabelFont (juce::Label&)
{
    return juce::Font (13.0f, juce::Font::plain).withExtraKerningFactor (0.08f);
}

void MdottyLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                           juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (6.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Determine accent colour from slider properties (set per-instance).
    juce::Colour accent = MdottyColours::accentPurple;
    if (auto* v = slider.getProperties().getVarPointer ("accentColour"))
        accent = juce::Colour ((juce::uint32) (juce::int64) *v);

    // Background track ring.
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (MdottyColours::trackDim);
    g.strokePath (track, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Glow value arc.
    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (accent.withAlpha (0.25f));
    g.strokePath (valueArc, juce::PathStrokeType (9.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (accent);
    g.strokePath (valueArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Knob body.
    auto knobRadius = radius * 0.62f;
    juce::ColourGradient grad (MdottyColours::knobBody.brighter (0.15f), centre.x, centre.y - knobRadius,
                                MdottyColours::knobBody.darker (0.3f), centre.x, centre.y + knobRadius, false);
    g.setGradientFill (grad);
    g.fillEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.drawEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.0f);

    // Pointer.
    juce::Path pointer;
    float pointerLen = knobRadius * 0.75f;
    float pointerThickness = 2.5f;
    pointer.addRoundedRectangle (-pointerThickness * 0.5f, -pointerLen, pointerThickness, pointerLen * 0.55f, 1.5f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
    g.setColour (accent);
    g.fillPath (pointer);
}

void MdottyLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float minSliderPos, float maxSliderPos,
                                          const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    juce::ignoreUnused (minSliderPos, maxSliderPos, style);
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height);
    auto trackBounds = bounds.reduced (0.0f, bounds.getHeight() * 0.35f);

    g.setColour (MdottyColours::trackDim);
    g.fillRoundedRectangle (trackBounds, trackBounds.getHeight() * 0.5f);

    juce::Colour accent = MdottyColours::accentTeal;
    if (auto* v = slider.getProperties().getVarPointer ("accentColour"))
        accent = juce::Colour ((juce::uint32) (juce::int64) *v);

    auto fillBounds = trackBounds.withRight (sliderPos);
    juce::ColourGradient grad (MdottyColours::accentPurple, trackBounds.getX(), 0,
                                accent, trackBounds.getRight(), 0, false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (fillBounds, trackBounds.getHeight() * 0.5f);

    auto handleRadius = bounds.getHeight() * 0.4f;
    g.setColour (juce::Colours::white);
    g.fillEllipse (sliderPos - handleRadius * 0.5f, bounds.getCentreY() - handleRadius * 0.5f,
                   handleRadius, handleRadius);
}

void MdottyLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                          bool isHighlighted, bool /*isDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);
    bool on = button.getToggleState();

    auto pillBounds = bounds.withHeight (juce::jmin (bounds.getHeight(), 22.0f)).withCentre (bounds.getCentre());
    g.setColour (on ? MdottyColours::accentTeal.withAlpha (0.85f) : MdottyColours::trackDim);
    g.fillRoundedRectangle (pillBounds, pillBounds.getHeight() * 0.5f);
    if (isHighlighted)
        g.setColour (juce::Colours::white.withAlpha (0.08f)), g.fillRoundedRectangle (pillBounds, pillBounds.getHeight() * 0.5f);

    float knobD = pillBounds.getHeight() - 4.0f;
    float knobX = on ? pillBounds.getRight() - knobD - 2.0f : pillBounds.getX() + 2.0f;
    g.setColour (juce::Colours::white);
    g.fillEllipse (knobX, pillBounds.getY() + 2.0f, knobD, knobD);

    if (button.getButtonText().isNotEmpty())
    {
        g.setColour (MdottyColours::textDim);
        g.setFont (12.0f);
        g.drawText (button.getButtonText(), bounds.withTrimmedTop ((int) pillBounds.getHeight() + 4),
                    juce::Justification::centredTop);
    }
}

void MdottyLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& backgroundColour,
                                              bool isHighlighted, bool isDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    auto colour = backgroundColour;
    if (isDown) colour = colour.brighter (0.1f);
    else if (isHighlighted) colour = colour.brighter (0.05f);

    g.setColour (colour);
    g.fillRoundedRectangle (bounds, 6.0f);
}
