# erdGameTools

`erdGameTools` 是一个面向《艾尔登法环》`1.16.1` 的本地 DLL mod 工程。

当前仓库的主要目标是把中文 CT 表里的高频功能逐步迁移成可维护的 C++ 插件，并通过游戏原生菜单提供入口，而不是只依赖独立悬浮菜单。

## 截图

### 原生菜单

![原生菜单一级](./erdGameTools/men1.png)

![原生菜单二级](./erdGameTools/menu2.png)

![原生菜单三级](./erdGameTools/menu3.png)

### 伤害弹字

![伤害弹字效果](./erdGameTools/hit%20numer%20show.png)

## 项目说明

当前仓库主要包含两条功能线：

- `erdGameTools/`
  原生赐福菜单插件。参考 ThomasJClark 系列项目的 DLL 结构，把菜单直接插入游戏内容内。
- `Source/`
  早期 `ERDMod` 主工程与通用功能模块。`erdGameTools` 复用了这里的大量底层能力，例如角色标志、参数补丁、解锁功能、姿态条采集、NPC 菜单读取等。

当前 `erdGameTools` 已包含：

- 原生赐福菜单注入
- 中英文自动切换与外置语言文本
- 架势条与伤害弹字显示
- 游戏修改类开关
- 解锁类功能
- 快捷入口菜单
- 本地配置持久化与进档自动重应用

## 目录说明

```text
.
|-- erdGameTools/               # 当前主插件工程
|-- Source/                     # 通用功能与旧主工程代码
|-- Resources/                  # 外置菜单、脚本、下拉表资源
|-- Config/                     # 旧 ERDMod 配置
|-- scripts/                    # 构建与辅助脚本
`-- dist/                       # 打包输出
```

和当前 `erdGameTools` 直接相关的关键目录：

```text
erdGameTools/
|-- CMakeLists.txt
|-- config.ini
|-- Resources/
|   `-- Lang/
|       |-- en-US.txt
|       `-- zh-CN.txt
`-- src/
    |-- dllmain.cpp
    |-- grace_test_config.cpp
    |-- grace_test_localization.cpp
    |-- grace_test_messages.cpp
    |-- grace_test_overlay.cpp
    |-- grace_test_runtime.cpp
    `-- grace_test_talkscript.cpp
```

## 构建说明

### 配置

```powershell
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -S .\erdGameTools -B .\erdGameTools\build-erdGameTools
```

### 编译

```powershell
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build .\erdGameTools\build-erdGameTools --config Release
```

### 输出

编译产物：

- `erdGameTools\build-erdGameTools\Release\erdGameTools.dll`

当前打包目录：

- `dist\erdGameTools\erdGameTools.dll`
- `dist\erdGameTools\config.ini`
- `dist\erdGameTools\Resources\Lang\*.txt`

## 配置说明

当前插件配置文件路径：

- `erdGameTools\config.ini`
- 打包副本：`dist\erdGameTools\config.ini`

配置分为：

- `[GameMods]`
- `[Unlocks]`
- `[Localization]`

其中：

- `ui_language=auto` 会根据游戏当前文本语言自动选择 `zh-CN` 或 `en-US`
- 也可以手动写成 `zh-CN` 或 `en-US`

## 引用说明

本仓库使用或参考了以下开源项目与组件：

- [ThomasJClark/elden-ring-transmog](https://github.com/ThomasJClark/elden-ring-transmog)
- [ThomasJClark/elden-ring-glorious-merchant](https://github.com/ThomasJClark/elden-ring-glorious-merchant)
- [ThomasJClark/elden-x](https://github.com/ThomasJClark/elden-x)
- [gabime/spdlog](https://github.com/gabime/spdlog)
- [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook)
- [ocornut/imgui](https://github.com/ocornut/imgui)

## 参考项目说明

### `elden-ring-transmog`

本项目主要借鉴了它的：

- ModEngine 扩展 DLL 组织方式
- `DllMain -> worker thread -> modutils::initialize -> hook -> enable_hooks` 初始化流程
- 原生 `talkscript` 菜单注入思路
- 自定义 `event_text_for_talk` 文本拦截方式

### `elden-ring-glorious-merchant`

本项目主要借鉴了它的：

- 原生菜单扩展的工程结构
- 与游戏菜单脚本交互的设计方式
- 原生菜单型 mod 的目录布局和初始化风格

### 说明

本仓库不是以上项目的镜像，也不是直接改名发布版本。

当前实现是在参考其架构思路后，结合中文 CT 表功能迁移需求，重新组织出的本地工程。

## 当前仓库用途

这个仓库当前主要用于：

- 备份 `erdGameTools` 与 `ERDMod` 的当前开发状态
- 持续迁移中文 CT 表功能到 DLL
- 逐步把功能从 ImGui 菜单迁移到原生游戏菜单
- 为后续继续扩展语言包、快捷入口、参数工具和更多原生菜单功能做基础
