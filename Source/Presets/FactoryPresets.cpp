#include "FactoryPresets.h"
#include "../PluginProcessor.h"
#include <algorithm>

namespace
{
    const std::vector<FactoryPreset> presets = {
        // -------- Start Here --------
        { "Start Here", "Init", {
            { ParamIDs::dryWet, 0.5f }, { ParamIDs::stretchSpeed, 1.0f }, { ParamIDs::density, 20.0f },
            { ParamIDs::size, 120.0f }, { ParamIDs::pitch, 0.0f }
        } },
        { "Start Here", "Gentle Grain", {
            { ParamIDs::dryWet, 0.35f }, { ParamIDs::stretchSpeed, 0.9f }, { ParamIDs::density, 14.0f },
            { ParamIDs::size, 180.0f }, { ParamIDs::scale, 0.15f }
        } },

        // -------- Stretch & Smear --------
        { "Stretch & Smear", "The Great Slowness", {
            { ParamIDs::dryWet, 0.85f }, { ParamIDs::stretchSpeed, 0.25f }, { ParamIDs::density, 10.0f },
            { ParamIDs::size, 600.0f }, { ParamIDs::scale, 0.4f }, { ParamIDs::shape, 0.0f }
        } },
        { "Stretch & Smear", "Chordal Clouds", {
            { ParamIDs::dryWet, 0.9f }, { ParamIDs::stretchSpeed, 0.4f }, { ParamIDs::density, 8.0f },
            { ParamIDs::size, 900.0f }, { ParamIDs::pan, 0.6f }
        } },
        { "Stretch & Smear", "Fractured", {
            { ParamIDs::dryWet, 0.7f }, { ParamIDs::stretchSpeed, 1.6f }, { ParamIDs::density, 45.0f },
            { ParamIDs::size, 60.0f }, { ParamIDs::retrigger, 0.5f }
        } },

        // -------- Pitch --------
        { "Pitch", "Blurred 5ths", {
            { ParamIDs::dryWet, 0.6f }, { ParamIDs::pitch, 7.0f }, { ParamIDs::density, 18.0f },
            { ParamIDs::size, 200.0f }
        } },
        { "Pitch", "Pitched And Glitched", {
            { ParamIDs::dryWet, 0.65f }, { ParamIDs::pitch, -12.0f }, { ParamIDs::density, 30.0f },
            { ParamIDs::size, 40.0f }, { ParamIDs::retrigger, 0.7f }
        } },

        // -------- Delay --------
        { "Delay", "Forever Delay", {
            { ParamIDs::dryWet, 0.4f }, { ParamIDs::delayTime, 480.0f }, { ParamIDs::delayFeedback, 0.55f },
            { ParamIDs::delayLP, 6000.0f }
        } },
        { "Delay", "Undul8 Delay", {
            { ParamIDs::dryWet, 0.45f }, { ParamIDs::delayTime, 260.0f }, { ParamIDs::delayFeedback, 0.4f },
            { ParamIDs::mod1Rate, 0.6f }, { ParamIDs::mod1Humanize, 0.3f }
        } },

        // -------- Drums --------
        { "Drums", "Bounce Detuner", {
            { ParamIDs::dryWet, 0.5f }, { ParamIDs::pitch, -3.0f }, { ParamIDs::density, 25.0f },
            { ParamIDs::size, 80.0f }, { ParamIDs::retrigger, 0.8f }
        } },
        { "Drums", "Skitter Flux", {
            { ParamIDs::dryWet, 0.6f }, { ParamIDs::density, 55.0f }, { ParamIDs::size, 30.0f },
            { ParamIDs::scale, 0.5f }, { ParamIDs::retrigger, 1.0f }
        } },

        // -------- Glitch --------
        { "Glitch", "Stagger Step", {
            { ParamIDs::dryWet, 0.75f }, { ParamIDs::density, 60.0f }, { ParamIDs::size, 25.0f },
            { ParamIDs::offset, 0.3f }, { ParamIDs::retrigger, 1.0f }
        } },
        { "Glitch", "Crusher Switcher", {
            { ParamIDs::dryWet, 0.8f }, { ParamIDs::density, 70.0f }, { ParamIDs::size, 18.0f },
            { ParamIDs::pitch, -5.0f }
        } },

        // -------- Vocals --------
        { "Vocals", "Heaven Reveal", {
            { ParamIDs::dryWet, 0.5f }, { ParamIDs::stretchSpeed, 0.6f }, { ParamIDs::density, 16.0f },
            { ParamIDs::size, 260.0f }, { ParamIDs::delayTime, 400.0f }, { ParamIDs::delayFeedback, 0.3f }
        } },
        { "Vocals", "Tremble Reflection", {
            { ParamIDs::dryWet, 0.45f }, { ParamIDs::pitch, 12.0f }, { ParamIDs::density, 14.0f },
            { ParamIDs::size, 220.0f }
        } },

        // -------- Guitar & Keys --------
        { "Guitar & Keys", "Old Tapes", {
            { ParamIDs::dryWet, 0.4f }, { ParamIDs::stretchSpeed, 0.85f }, { ParamIDs::pitch, -1.0f },
            { ParamIDs::density, 12.0f }, { ParamIDs::size, 300.0f }
        } },
        { "Guitar & Keys", "Major Shimmer", {
            { ParamIDs::dryWet, 0.55f }, { ParamIDs::pitch, 12.0f }, { ParamIDs::delayTime, 500.0f },
            { ParamIDs::delayFeedback, 0.5f }
        } },

        // -------- Bass --------
        { "Bass", "Sub Smear", {
            { ParamIDs::dryWet, 0.35f }, { ParamIDs::pitch, -12.0f }, { ParamIDs::stretchSpeed, 0.7f },
            { ParamIDs::size, 350.0f }
        } },
    };

    std::vector<juce::String> buildCategoryList()
    {
        std::vector<juce::String> cats;
        for (auto& p : presets)
            if (std::find (cats.begin(), cats.end(), p.category) == cats.end())
                cats.push_back (p.category);
        return cats;
    }
}

namespace FactoryPresets
{
    const std::vector<juce::String>& getCategories()
    {
        static std::vector<juce::String> cats = buildCategoryList();
        return cats;
    }

    std::vector<const FactoryPreset*> getPresetsForCategory (const juce::String& category)
    {
        std::vector<const FactoryPreset*> result;
        for (auto& p : presets)
            if (p.category == category)
                result.push_back (&p);
        return result;
    }

    const std::vector<FactoryPreset>& getAll() { return presets; }
}
