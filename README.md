# erdGameTools

English | [简体中文](./README.zh-CN.md)

`erdGameTools` is a native Elden Ring in-game menu mod for game version `1.16.1`.

Instead of relying on a separate trainer window, it injects a new `erdGameTools` entry into the Site of Grace menu and exposes gameplay toggles, unlock helpers, posture bars, damage popups, and quick-access actions through the game's own UI.

GitHub repository:

- [https://github.com/ankerlcg1996/-Elden_Ring_game_tools](https://github.com/ankerlcg1996/-Elden_Ring_game_tools)
- Feature overview (bilingual): [MOD_FEATURES.md](./MOD_FEATURES.md)

## Screenshots

![Main menu](./men1.png)

![Second-level menu](./menu2.png)

![Third-level menu](./menu3.png)

![Damage popup](./hit%20numer%20show.png)

## Main Features

### 1. Native in-game menu integration

- Adds an `erdGameTools` root entry directly into the Site of Grace menu.
- Supports nested menu structure instead of a separate overlay trainer UI.
- Uses external language files and auto-switches between Chinese and English.

### 2. Gameplay mod toggles

Available toggles in the in-game menu:

- Weightless Equipment
- Infinite Item Usage
- No Damage
- No Stamina Cost
- Free Purchase
- No Craft Material Cost
- No Upgrade Material Cost
- No Magic Requirements
- All Magic Uses One Slot
- Mount Anywhere
- Spirit Ashes Anywhere
- Posture Bars
- Damage Popups

These toggles are persistent. Once enabled or disabled in-game, their state is saved locally and automatically re-applied the next time a character is loaded.

### 3. Unlock helpers

Available unlock actions:

- Unlock All Maps
- Unlock All Cookbooks
- Unlock All Whetblades
- Unlock All Graces
- Unlock All Summoning Pools
- Unlock All Colosseums
- Unlock All Gestures

These options are also persistent and automatically re-applied after loading into the game.

### 4. Posture bar overlay

Integrated posture bar rendering is built into this project and no longer depends on a separate PostureBar DLL.

Current behavior:

- Boss posture bars are rendered with a wide on-screen bar.
- Normal enemy posture bars are rendered above the target in the world.
- The bar style uses a warm orange fill with a darker double-border style.
- The overlay is hooked through DirectX 12.

### 5. Damage popup system

Damage popups are rendered near tracked enemies based on real HP changes instead of relying only on the game's own UI effects.

Current behavior:

- Each damage popup stays visible for 5 seconds.
- Popups slowly rise upward and fade out over time.
- Up to the most recent 5 damage values can be visible at once per target stack.
- Damage size and color are split into 10 tiers.
- Instant-kill cases are handled with a fallback path so final-hit damage can still appear even when the target UI disappears immediately.

### 6. Quick Access menu

Quick Access entries are loaded from an external file:

- `Resources/NpcMenus.zh.txt`

This allows you to expand or modify shortcut actions without recompiling the DLL.

### 7. Localization system

Language strings are loaded from external files:

- `Resources/Lang/en-US.txt`
- `Resources/Lang/zh-CN.txt`

Behavior:

- `ui_language=auto` detects the game text language automatically.
- You can also force a language manually through `erdGameTools.ini`.
- The system is designed so more languages can be added later with extra text files.

### 8. Persistent config

Configuration is stored in:

- `erdGameTools.ini`

Current sections:

- `[GameMods]`
- `[Unlocks]`
- `[Localization]`

The mod saves toggle states immediately after you change them in the in-game menu.

## Installation

1. Prepare `Elden Mod Loader`.
2. Download this mod and extract the included `mods` folder into your game root directory.
3. Open the mod config file in the game root, usually `config_eldenring.toml`, with Notepad.
4. Find the `external_dlls = [` section and add this mod to the list.

Single DLL example:

```toml
external_dlls = [
  "erdGameTools.dll",
]
```

Multiple DLL example:

```toml
external_dlls = [
  "ertransmogrify.dll",
  "erdGameTools.dll",
]
```

5. Double-click `launchmod_eldenring.bat` to start the game.

## Build

```powershell
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -S . -B .\build-selfcontained -G "Visual Studio 17 2022" -A x64
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build .\build-selfcontained --config Release
```

Build output:

- `build-selfcontained\Release\erdGameTools.dll`

## Project Layout

```text
.
|-- CMakeLists.txt
|-- erdGameTools.ini
|-- README.md
|-- README.zh-CN.md
|-- Resources/
|   |-- Lang/
|   |   |-- en-US.txt
|   |   `-- zh-CN.txt
|   |-- NpcMenus.zh.txt
|   `-- ParamScripts.zh.txt
|-- Source/
|   |-- Common.hpp
|   |-- Features/
|   |-- Game/
|   |-- Main/
|   `-- ThirdParty/
`-- src/
    |-- dllmain.cpp
    |-- grace_test_config.cpp
    |-- grace_test_localization.cpp
    |-- grace_test_messages.cpp
    |-- grace_test_overlay.cpp
    |-- grace_test_runtime.cpp
    `-- grace_test_talkscript.cpp
```

## References

Primary reference projects:

- [ThomasJClark/elden-ring-transmog](https://github.com/ThomasJClark/elden-ring-transmog)
- [ThomasJClark/elden-ring-glorious-merchant](https://github.com/ThomasJClark/elden-ring-glorious-merchant)

Supporting libraries:

- [ThomasJClark/elden-x](https://github.com/ThomasJClark/elden-x)
- [gabime/spdlog](https://github.com/gabime/spdlog)
- [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook)
- [ocornut/imgui](https://github.com/ocornut/imgui)

What was referenced:

- `elden-ring-transmog`: native talkscript menu injection, message interception, and ModEngine extension DLL startup flow
- `elden-ring-glorious-merchant`: native in-game menu structure and content-integrated DLL organization

This repository is a separate implementation focused on CT feature migration, native menu integration, and in-game tooling for Elden Ring.
