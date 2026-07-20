#include "PresetBrowser.h"
#include "MdottyLookAndFeel.h"

PresetBrowser::PresetBrowser (juce::AudioProcessorValueTreeState& state) : apvts (state)
{
    categoryList.setModel (&categoryModel);
    presetList.setModel (&presetModel);
    categoryList.setRowHeight (34);
    presetList.setRowHeight (26);
    addAndMakeVisible (categoryList);
    addAndMakeVisible (presetList);

    closeButton.onClick = [this] { if (onClose) onClose(); };
    addAndMakeVisible (closeButton);

    if (! FactoryPresets::getCategories().empty())
        selectedCategory = FactoryPresets::getCategories().front();
}

void PresetBrowser::resized()
{
    auto bounds = getLocalBounds().reduced (12);
    auto top = bounds.removeFromTop (28);
    closeButton.setBounds (top.removeFromRight (28));

    categoryList.setBounds (bounds.removeFromLeft (bounds.getWidth() / 3));
    bounds.removeFromLeft (8);
    presetList.setBounds (bounds);
}

void PresetBrowser::paint (juce::Graphics& g)
{
    g.setColour (MdottyColours::bg1);
    g.fillRect (getLocalBounds());
}

void PresetBrowser::applyPreset (const FactoryPreset& preset)
{
    for (auto& kv : preset.values)
        if (auto* p = apvts.getParameter (kv.first))
        {
            auto range = apvts.getParameterRange (kv.first);
            p->setValueNotifyingHost (range.convertTo0to1 (kv.second));
        }
}

void PresetBrowser::CategoryModel::paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected)
{
    const auto& cats = FactoryPresets::getCategories();
    if (row < 0 || row >= (int) cats.size()) return;

    if (selected || cats[(size_t) row] == owner.selectedCategory)
        g.fillAll (MdottyColours::panel.brighter (0.06f));

    g.setColour (MdottyColours::textBright);
    g.setFont (13.0f);
    g.drawText (cats[(size_t) row].toUpperCase(), 12, 0, w - 20, h, juce::Justification::centredLeft);
}

void PresetBrowser::CategoryModel::listBoxItemClicked (int row, const juce::MouseEvent&)
{
    const auto& cats = FactoryPresets::getCategories();
    if (row < 0 || row >= (int) cats.size()) return;
    owner.selectedCategory = cats[(size_t) row];
    owner.presetList.updateContent();
    owner.presetList.repaint();
    owner.categoryList.repaint();
}

int PresetBrowser::PresetModel::getNumRows()
{
    if (owner.selectedCategory.isEmpty()) return 0;
    return (int) FactoryPresets::getPresetsForCategory (owner.selectedCategory).size();
}

void PresetBrowser::PresetModel::paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected)
{
    auto list = FactoryPresets::getPresetsForCategory (owner.selectedCategory);
    if (row < 0 || row >= (int) list.size()) return;

    if (selected) g.fillAll (MdottyColours::panel.brighter (0.08f));

    g.setColour (MdottyColours::accentTeal);
    g.setFont (12.0f);
    g.drawText (juce::String (juce::CharPointer_UTF8 ("\u2605")), 10, 0, 16, h, juce::Justification::centred);

    g.setColour (MdottyColours::textDim);
    g.setFont (13.0f);
    g.drawText (list[(size_t) row]->name.toUpperCase(), 32, 0, w - 40, h, juce::Justification::centredLeft);
}

void PresetBrowser::PresetModel::listBoxItemClicked (int row, const juce::MouseEvent&)
{
    auto list = FactoryPresets::getPresetsForCategory (owner.selectedCategory);
    if (row < 0 || row >= (int) list.size()) return;
    owner.applyPreset (*list[(size_t) row]);
}
