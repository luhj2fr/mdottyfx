#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Presets/FactoryPresets.h"

class PresetBrowser : public juce::Component
{
public:
    explicit PresetBrowser (juce::AudioProcessorValueTreeState& state);

    void resized() override;
    void paint (juce::Graphics&) override;

    std::function<void()> onClose;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::ListBox categoryList { "categories", nullptr };
    juce::ListBox presetList   { "presets", nullptr };
    juce::TextButton closeButton { "X" };

    struct CategoryModel : public juce::ListBoxModel
    {
        PresetBrowser& owner;
        explicit CategoryModel (PresetBrowser& o) : owner (o) {}
        int getNumRows() override { return (int) FactoryPresets::getCategories().size(); }
        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override;
        void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    } categoryModel { *this };

    struct PresetModel : public juce::ListBoxModel
    {
        PresetBrowser& owner;
        explicit PresetModel (PresetBrowser& o) : owner (o) {}
        int getNumRows() override;
        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override;
        void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    } presetModel { *this };

    juce::String selectedCategory;
    void applyPreset (const FactoryPreset& preset);

    friend struct CategoryModel;
    friend struct PresetModel;
};
