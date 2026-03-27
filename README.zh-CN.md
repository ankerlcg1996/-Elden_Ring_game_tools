# erdGameTools

[English](./README.md) | 简体中文

`erdGameTools` 是一个面向《艾尔登法环》`1.16.1` 版本的原生游戏内菜单模组。

它不是单独弹出一个训练器窗口，而是直接把 `erdGameTools` 插入到赐福菜单中，在游戏原生 UI 里提供功能开关、解锁功能、架势条、伤害弹字和快捷入口。

GitHub 仓库地址：

- [https://github.com/ankerlcg1996/-Elden_Ring_game_tools](https://github.com/ankerlcg1996/-Elden_Ring_game_tools)
- 双语功能说明： [MOD_FEATURES.md](./MOD_FEATURES.md)

## 截图

![主菜单](./men1.png)

![二级菜单](./menu2.png)

![三级菜单](./menu3.png)

![伤害弹字](./hit%20numer%20show.png)

## 详细功能介绍

### 1. 原生赐福菜单注入

- 在赐福菜单中新增 `erdGameTools` 根菜单。
- 支持多级菜单结构，不依赖独立悬浮训练器窗口。
- 菜单文本支持中英文切换，并且文本内容外置。

### 2. 游戏修改开关

当前已接入的开关包括：

- 装备无重量
- 道具不消耗
- 无伤
- 不耗精力
- 商店免费购买
- 制作不消耗材料
- 强化不消耗材料
- 魔法没有使用需求
- 所有魔法只占一个栏位
- 随处骑马
- 随处召唤骨灰
- 架势条显示
- 伤害显示

这些开关都会本地持久化保存。你在游戏里开关一次后，下次重新进档会自动恢复并重新应用。

### 3. 一次性解锁功能

当前已接入的解锁功能包括：

- 解锁全部地图
- 解锁全部制作笔记
- 解锁全部砥石刀
- 解锁全部赐福
- 解锁全部召唤池
- 解锁全部竞技场
- 解锁全部肢体动作

这些功能同样支持持久化记忆，进档后会自动再次触发。

### 4. 架势条显示

项目已经融合了姿态条相关能力，不再依赖独立的 PostureBar DLL。

当前行为：

- Boss 目标会显示较宽的屏幕架势条。
- 普通敌人会在目标附近显示世界空间架势条。
- 架势条采用橙色填充和更偏魂味的深色双层描边。
- Overlay 基于 DirectX 12 hook 实现。

### 5. 伤害弹字系统

伤害弹字基于目标真实 HP 变化生成，而不是只依赖游戏原本的显示动画。

当前行为：

- 每条伤害弹字持续显示 5 秒。
- 显示期间会缓慢上移并逐渐淡出。
- 同一目标最多同时显示最近 5 条伤害。
- 按伤害数值分成 10 档，字号和颜色逐级增强。
- 对秒杀情况做了兜底处理，即使目标 UI 立即消失，也尽量补出最后一击伤害。

### 6. 快捷入口

快捷入口菜单来自外置文件：

- `Resources/NpcMenus.zh.txt`

你可以通过修改这个文件来扩展或调整快捷入口，而不需要重新编译 DLL。

### 7. 本地化系统

语言文件外置在：

- `Resources/Lang/en-US.txt`
- `Resources/Lang/zh-CN.txt`

当前行为：

- `ui_language=auto` 会根据游戏文本自动切换语言。
- 也可以在 `erdGameTools.ini` 中手动指定语言。
- 后续如果要扩展更多语言，只需要增加新的语言文本文件。

### 8. 配置与持久化

配置文件位于：

- `erdGameTools.ini`

当前配置段包括：

- `[GameMods]`
- `[Unlocks]`
- `[Localization]`

在菜单中修改后的开关状态会立即写入配置文件。

## 使用说明

1. 先准备好 `Elden Mod Loader`。
2. 下载本模组，并把压缩包中的 `mods` 目录解压到游戏根目录。
3. 在游戏根目录找到模组配置文件，一般是 `config_eldenring.toml`，用记事本打开。
4. 找到以 `external_dlls = [` 开头的配置段，把本模组加入列表。

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

5. 双击 `launchmod_eldenring.bat` 开始游戏。

## 编译

```powershell
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -S . -B .\build-selfcontained -G "Visual Studio 17 2022" -A x64
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build .\build-selfcontained --config Release
```

输出位置：

- `build-selfcontained\Release\erdGameTools.dll`

## 项目结构

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

## 参考项目

主要参考项目：

- [ThomasJClark/elden-ring-transmog](https://github.com/ThomasJClark/elden-ring-transmog)
- [ThomasJClark/elden-ring-glorious-merchant](https://github.com/ThomasJClark/elden-ring-glorious-merchant)

依赖或参考的库：

- [ThomasJClark/elden-x](https://github.com/ThomasJClark/elden-x)
- [gabime/spdlog](https://github.com/gabime/spdlog)
- [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook)
- [ocornut/imgui](https://github.com/ocornut/imgui)

具体借鉴点：

- `elden-ring-transmog`：原生 talkscript 菜单注入、消息拦截、ModEngine 扩展 DLL 启动流程
- `elden-ring-glorious-merchant`：原生游戏菜单结构设计、内容整合式 DLL 模组组织方式

本仓库是在这些思路基础上，结合中文 CT 功能迁移需求，重新实现的独立项目。
