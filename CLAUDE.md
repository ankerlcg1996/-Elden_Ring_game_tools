# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Scope

- This workspace contains the real `erdGameTools` project at the repository root, plus reference/mirror projects in `TarnishedTool-*`, `Elden-Ring-CT-TGA-master`, and `Erd-Tools-CPP-main`.
- Unless the user explicitly asks otherwise, keep changes scoped to the root project defined by `CMakeLists.txt`, `src/`, `Source/`, `Resources/`, and `config.ini`.

## Build and validation

Primary build workflow (Visual Studio 2022 toolchain + CMake):

```powershell
cmake -S . -B .\build-selfcontained -G "Visual Studio 17 2022" -A x64
cmake --build .\build-selfcontained --config Release
```

Useful variants:

```powershell
cmake --build .\build-selfcontained --config Debug
cmake --build .\build-selfcontained --config Release --target erdGameTools
```

- Output DLL: `build-selfcontained\Release\erdGameTools.dll`
- `CMakeLists.txt` runs a post-build copy step for `Resources/NpcMenus.zh.txt`, `Resources/ParamScripts.zh.txt`, and `Resources/Lang/*`, so local verification should use the copied files next to the built DLL.
- There is no repo-configured lint/format command in this snapshot (`.clang-format` and `.clang-tidy` are absent).
- There is no automated test suite or `ctest` target. There is no single-test command; files named `src/grace_test_*` are runtime modules, not standalone tests.
- Minimum validation for behavior changes: build `Release`, load the DLL through Elden Mod Loader on game version `1.16.1`, and verify changed menu/overlay behavior in game.

## Architecture

### Runtime bootstrap

- `src/dllmain.cpp` is the DLL entrypoint. It initializes logging, config, and localization from the DLL folder, installs message/talkscript hooks, and starts the runtime worker thread.
- `modengine_ext_init()` exposes the DLL as a ModEngine-compatible extension.
- Logging is split between `spdlog` during bootstrap/hooking and `Source/Main/Logger.cpp` for feature/runtime logging; both write under the DLL-adjacent `logs/` directory.

### Runtime orchestration

- `src/grace_test_runtime.cpp` is the main orchestration loop.
- It waits for FD4 singletons to become discoverable, initializes the DX12 overlay layer, detects when a character/save is actually loaded, reapplies persisted settings on load, and ticks feature modules on a polling loop.
- `Source/Main/FeatureStatus.hpp` is the central state bus. Menu callbacks, runtime feature ticks, and overlay rendering communicate through these shared atomics.

### Native Site of Grace menu integration

- `src/grace_test_messages.cpp` hooks the game’s talk message lookup and returns custom text for the injected menu message IDs.
- `src/grace_test_talkscript.cpp` hooks `EzState::EnterState`, detects the Site of Grace state group, injects the top-level `erdGameTools` option, and rebuilds the root/game mods/unlocks/quick access menus each time the relevant state is entered.
- Quick Access entries are data-driven from `Resources/NpcMenus.zh.txt`.

### Config and localization

- `src/grace_test_config.cpp` reads and writes `config.ini` next to the DLL. Menu toggles persist immediately after callback execution and are re-applied on the next character load.
- `src/grace_test_localization.cpp` loads language files from `Resources/Lang/`, supports `ui_language=auto`, and falls back to `en-US.txt` if a requested language file is missing.
- `src/grace_test_messages.cpp` auto-detects the game text language by probing a vanilla talk message, which is why language switching is tied to in-game text state rather than only config.

### Game integration layer

- `Source/Game/SingletonRegistry.cpp` reconstructs FD4 singleton storage by AOB-scanning the game module and exposes singleton object pointers by simple name.
- `Source/Game/Memory.hpp`, `Source/Game/EventFlags.*`, and `Source/Game/Params.hpp` provide the low-level memory, flag, and param helpers used by the gameplay features.
- The runtime loop treats successful singleton discovery as the boundary between “DLL loaded” and “game systems ready”.

### Feature modules

- `Source/Features/CharacterFlags.cpp` and `Source/Features/ParamPatches.cpp` handle continuous gameplay toggles.
- `Source/Features/Unlocks.cpp` handles one-shot unlock actions and tracks whether each unlock has already been applied during the current load session.
- `Source/Features/InspectorTools.cpp` feeds the quick-access / inspection flows.
- `Source/Features/PostureBars.cpp` collects posture and HP snapshots from game state.
- `src/grace_test_overlay.cpp` renders posture bars and damage popups through a DirectX 12 overlay built with ImGui and MinHook.

### Third-party code

- Vendored dependencies live under `Source/ThirdParty/`.
- Avoid editing `ImGui` or `MinHook` sources unless the task is explicitly about dependency changes.

## Code organization and naming

- Language target is C++23.
- Use 4-space indentation and keep formatting aligned with nearby code.
- Preserve the existing naming split:
  - `PascalCase` classes/files in `Source/`
  - existing `grace_test_*` snake_case module names in `src/`
- Keep the common `Xxx.hpp` + `Xxx.cpp` pairing pattern in `Source/`.

## Repository references

- `README.md` and `README.zh-CN.md` document the user-facing feature set, installation expectations, and build commands.
- The root `AGENTS.md` already notes that `TarnishedTool-*`, `Elden-Ring-CT-TGA-master`, and `Erd-Tools-CPP-main` are reference/mirror projects; treat them as read-only context unless the user explicitly targets them.
