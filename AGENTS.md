# Repository Guidelines

## Project Structure & Module Organization
This repository is centered on the native `erdGameTools` DLL:

- `src/`: entrypoint/runtime wiring (`dllmain.cpp`, `grace_test_*` modules).
- `Source/Features/`: gameplay features (unlocks, posture bars, param patches, inspector tools).
- `Source/Game/`: game memory/event abstractions.
- `Source/Main/`: shared app infrastructure (for example logging).
- `Source/ThirdParty/`: vendored libraries (ImGui, MinHook); avoid edits unless explicitly upgrading dependencies.
- `Resources/`: runtime data copied next to the built DLL (`Lang/`, `NpcMenus.zh.txt`, `ParamScripts.zh.txt`).

`TarnishedTool-*`, `Elden-Ring-CT-TGA-master`, and `Erd-Tools-CPP-main` are reference/mirror projects; keep changes scoped unless a task explicitly targets them.

## Build, Test, and Development Commands
Primary workflow (Visual Studio 2022 toolchain + CMake):

```powershell
cmake -S . -B .\build-selfcontained -G "Visual Studio 17 2022" -A x64
cmake --build .\build-selfcontained --config Release
```

Output: `build-selfcontained\Release\erdGameTools.dll` (with copied `Resources/`).

Useful variants:

```powershell
cmake --build .\build-selfcontained --config Debug
cmake --build .\build-selfcontained --config Release --target erdGameTools
```

## Coding Style & Naming Conventions
- Language target is C++23 (see `CMakeLists.txt`).
- Use 4-space indentation and keep braces/style consistent with nearby code.
- Keep module pairing consistent: `Xxx.hpp` + `Xxx.cpp` in `Source/*`.
- Preserve existing naming patterns:
  - `PascalCase` for feature/game classes and files in `Source/`.
  - existing `grace_test_*` snake_case naming in `src/` should remain unchanged.
- Prefer small, focused feature edits; do not mix refactors with behavior changes.

## Testing Guidelines
There is no configured automated unit/integration test suite (`ctest` targets are not defined). Minimum validation for contributions:

1. Build succeeds in `Release`.
2. DLL loads via Elden Mod Loader on game version `1.16.1`.
3. Any changed menu/overlay behavior is verified in game and documented in the PR.

## Commit & Pull Request Guidelines
No repository-local Git history is available in this snapshot, so use clear Conventional Commit-style messages:

- `feat(menu): add quick access action`
- `fix(localization): fallback to en-US when key missing`

PRs should include:
- concise behavior summary and affected modules/paths,
- target game version and loader assumptions,
- before/after screenshots for UI/menu or overlay changes,
- repro/validation steps (what was tested in game).

## Feature Doc Sync Rule
- `PROJECT_FEATURES.md` is the authoritative feature tree for the current DLL behavior.
- Any feature addition/removal/rename must update `PROJECT_FEATURES.md` in the same change.
