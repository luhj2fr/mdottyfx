cmake_minimum_required(VERSION 3.22)

project(MDOTTYFX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Fetch JUCE (pinned to a stable tag). If you already have JUCE checked out
# as a submodule/subdirectory, replace this FetchContent block with:
#   add_subdirectory(JUCE)
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MDOTTYFX
    COMPANY_NAME        "ProdByMdotty"
    PLUGIN_MANUFACTURER_CODE Mdty
    PLUGIN_CODE         Mfx1
    FORMATS             VST3 Standalone
    PRODUCT_NAME        "MDOTTYFX"
    IS_SYNTH            FALSE
    NEEDS_MIDI_INPUT    FALSE
    NEEDS_MIDI_OUTPUT   FALSE
    IS_MIDI_EFFECT      FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES     Fx
)

target_sources(MDOTTYFX PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/GranularEngine.cpp
    Source/DSP/GrainDelay.cpp
    Source/UI/MdottyLookAndFeel.cpp
    Source/UI/MacroKnob.cpp
    Source/UI/XYPad.cpp
    Source/UI/DefaultPage.cpp
    Source/UI/GrainControlsPage.cpp
    Source/UI/PresetBrowser.cpp
    Source/Presets/FactoryPresets.cpp
)

target_include_directories(MDOTTYFX PRIVATE
    Source
    Source/DSP
    Source/UI
    Source/Presets
)

if (MSVC)
    target_compile_options(MDOTTYFX PRIVATE /utf-8)
endif()

target_compile_definitions(MDOTTYFX PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(MDOTTYFX PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
