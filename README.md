# erdGameTools

`erdGameTools` is a native Elden Ring menu mod for game version `1.16.1`.

This project injects an `erdGameTools` entry into the in-game Site of Grace menu, then exposes gameplay toggles, unlock helpers, posture bars, damage popups, and external quick-entry actions through the game's own UI instead of a separate floating trainer window.

## Screenshots

![Main menu](./men1.png)

![Second-level menu](./menu2.png)

![Third-level menu](./menu3.png)

![Damage popup](./hit%20numer%20show.png)

## Included Features

- Native Site of Grace menu injection
- Externalized language packs with automatic Chinese/English switching
- Posture bar overlay and damage popup overlay
- Persistent gameplay toggle states with auto-apply when loading into a save
- Persistent unlock actions with auto-apply when loading into a save
- External quick-entry menu items loaded from `Resources/NpcMenus.zh.txt`

## 使用说明

1. 下载最新版本的 `ModEngine2`，然后把它解压到任意目录。
2. 下载本模组，把 `erdGameTools.dll` 和 `config.ini` 解压到 `ModEngine-2.1.0.0-win64` 文件夹中。
3. 用记事本打开 `config_eldenring.toml`，找到以 `external_dlls = [` 开头的配置段，把本模组加入列表。

单个 DLL 示例：

```toml
external_dlls = [
  "erdGameTools.dll",
]
```

多个 DLL 示例：

```toml
external_dlls = [
  "ertransmogrify.dll",
  "erdGameTools.dll",
]
```

4. 双击 `launchmod_eldenring.bat` 开始游戏。

## Project Layout

```text
.
|-- CMakeLists.txt
|-- config.ini
|-- README.md
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

`Source/` contains the internal shared code and third-party components needed by the mod. It is now vendored directly into the repository root so GitHub opens straight into the actual project root.

## Build

```powershell
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -S . -B .\build-selfcontained -G "Visual Studio 17 2022" -A x64
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build .\build-selfcontained --config Release
```

Build output:

- `build-selfcontained\Release\erdGameTools.dll`

## Configuration

Configuration is stored in:

- `config.ini`

Current sections:

- `[GameMods]`
- `[Unlocks]`
- `[Localization]`

`ui_language=auto` will automatically choose `zh-CN` or `en-US` based on the detected in-game text language.

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
- `elden-ring-glorious-merchant`: native in-game menu mod layout and menu action organization

This repository is a separate implementation focused on Chinese CT feature migration and native menu expansion for Elden Ring tools.
