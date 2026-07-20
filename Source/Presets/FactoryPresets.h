#pragma once
#include <juce_core/juce_core.h>
#include <vector>

struct FactoryPreset
{
    juce::String category;
    juce::String name;
    // paramID -> value, applied on load. Anything not listed keeps its default.
    std::vector<std::pair<juce::String, float>> values;
};

namespace FactoryPresets
{
    const std::vector<juce::String>& getCategories();
    std::vector<const FactoryPreset*> getPresetsForCategory (const juce::String& category);
    const std::vector<FactoryPreset>& getAll();
}
