# MDOTTYFX
A granular time-stretch/pitch/delay effect plugin with a custom animated UI, built with JUCE + CMake.
Vendor: **ProdByMdotty**

## What's actually implemented
- Real granular DSP engine (`Source/DSP/GranularEngine.*`): capture buffer, spawns overlapping
  grains at a controllable density, independent stretch-speed vs. pitch, 4 window shapes,
  reverse, pan spread, size/scale jitter, retrigger jitter.
- Post-effect grain delay with feedback + HP/LP tone shaping (`Source/DSP/GrainDelay.*`).
- Fully custom `LookAndFeel` — no default JUCE sliders/knobs are visible anywhere
  (`Source/UI/MdottyLookAndFeel.*`).
- Animated XY macro pad with a rotating dotted starfield and a draggable puck bound to two
  parameters (`Source/UI/XYPad.*`) — this is what took the place of Portal's hero visual.
- Two-page layout switched by a left icon rail: the XY hero page (`DefaultPage`) and a full
  grain-controls page (`GrainControlsPage`) with speed/offset/retrigger/density/pitch/count/
  size/scale/pan + grain delay + two mod-rate/humanize knobs.
- A working preset browser (`Source/UI/PresetBrowser.*`) with categories and a starter bank of
  ~18 factory presets (`Source/Presets/FactoryPresets.cpp`) that actually set parameter values —
  add more by appending entries to that file, no UI code changes needed.
- CMake project using `FetchContent` to pull JUCE automatically, plus a GitHub Actions workflow
  that builds VST3 on both Windows and macOS runners and uploads it as a build artifact — matches
  the flow you described (push → Action builds → grab the VST3 → package your installer).

## What you still need to do
- **Tune the DSP to taste.** The granular algorithm is a real, working implementation, not a stub —
  but the exact character (Portal has years of tuning) is yours to dial in. Start in
  `GranularEngine::process()` and `spawnGrain()`.
- **Wire up host tempo sync.** The `*Sync` toggles (stretchSync, densitySync, delaySync) exist as
  parameters and are shown as pill toggles in the UI, but they aren't yet converting to
  beat-synced values — currently they're purely free-running (Hz/ms). Grab
  `getPlayHead()->getPosition()->getBpm()` in `processBlock` and map density/delay time to note
  divisions when the toggle is on.
- **Modulation routing.** `mod1Rate/Humanize` and `mod2Rate/Humanize` are real parameters with
  knobs, but they aren't yet connected to a modulation destination — Portal lets you drag them
  onto any control. For v1 I'd hardwire Mod1 → Density and Mod2 → Pitch as simple LFOs
  (`Source/DSP/` is a good place for a `ModLFO` class) rather than building a full drag-to-assign
  matrix, unless you want that as a v2.
- **App icon / installer.** You said you've already got the GitHub Actions → VST3 → Inno Setup
  .exe pipeline handled, so that's left alone here.

## Building locally
```bash
cmake -B build
cmake --build build --config Release
```
The VST3 will land in `build/MDOTTYFX_artefacts/Release/VST3/MDOTTYFX.vst3`.
A Standalone app is also built for quick testing without a DAW.

## Project map
```
CMakeLists.txt
Source/
  PluginProcessor.h/.cpp     — APVTS parameter layout, processBlock
  PluginEditor.h/.cpp        — top bar, icon rail, sidebar, page switching
  DSP/
    Grain.h                  — single grain + window shapes
    GranularEngine.h/.cpp    — the core granular engine
    GrainDelay.h/.cpp        — post-fx delay
  UI/
    MdottyLookAndFeel.h/.cpp — theme: colours, knob/toggle/slider painting
    MacroKnob.h/.cpp         — reusable labeled rotary knob
    XYPad.h/.cpp             — animated XY control
    DefaultPage.h/.cpp       — hero XY page
    GrainControlsPage.h/.cpp — full grain parameter grid
    PresetBrowser.h/.cpp     — category + preset list, applies FactoryPresets
  Presets/
    FactoryPresets.h/.cpp    — preset data (edit this to add more presets)
```
