# ERDMod

这是一个按照《艾尔登法环》常见 DLL mod 方式组织的 C++ 插件工程，结构参考了 `EldenRing-PostureBarMod` 这类正式 Elden Mod Loader 插件。

当前仓库已经包含：
- 标准的 `DllMain + MainThread` 启动链路
- 按 DLL 相对路径加载配置、日志和资源
- `Source/Main`、`Source/Game`、`Source/Features` 三层结构
- CT 风格的角色标志、事件标志解锁、参数表补丁功能
- DX12 + ImGui 状态/控制菜单
- 直接打包到 `mods` 目录的脚本

## 目录结构

```text
.
|-- Config
|   `-- ERDMod.ini
|-- Resources
|   `-- README.txt
|-- Source
|   |-- Common.hpp
|   |-- ERDMod.cpp
|   |-- ERDMod.hpp
|   |-- dllmain.cpp
|   |-- Features
|   |-- Game
|   `-- Main
|-- scripts
|   |-- build.ps1
|   `-- package_mod.ps1
`-- README.md
```

## 模块说明

- `Source/dllmain.cpp`
  只负责处理 `DLL_PROCESS_ATTACH / DETACH`，并启动工作线程。
- `Source/ERDMod.cpp`
  主线程入口，负责初始化路径、配置、日志、运行时和 overlay。
- `Source/Main`
  放插件框架代码，比如配置、日志、路径、运行循环和 ImGui 菜单。
- `Source/Game`
  放与游戏内存、FD4 单例、事件标志、参数表访问相关的底层逻辑。
- `Source/Features`
  放具体功能模块，例如无限 FP、无限道具、解锁地图、参数补丁。
- `Config`
  放插件配置文件。
- `Resources`
  预留给后续图片、字体、脚本或其他资源。

## 当前已实现功能

### Character Flags

- `infinite_fp`
  对应 CT 的 `NoFPConsumption`
- `infinite_items`
  对应 CT 的 `NoGoodsConsume`

### World Unlocks

- `unlock_all_maps`
- `unlock_all_cookbooks`
- `unlock_all_whetblades`

### Param Patches

- `faster_respawn`
- `warp_out_of_uncleared_minidungeons`

### ImGui Menu

- 游戏内 DX12 Overlay
- `Insert` 显示/隐藏菜单
- 可点击切换持续型功能
- 可点击触发一次性解锁/补丁功能

## 配置文件

默认路径：

```text
Config/ERDMod.ini
```

示例：

```ini
[General]
create_console=false
enable_file_log=true
show_message_box=false
message_box_text=ERDMod loaded.

[CharacterFlags]
infinite_fp=true
infinite_items=true

[WorldUnlocks]
unlock_all_maps=true
unlock_all_cookbooks=true
unlock_all_whetblades=true

[ParamPatches]
faster_respawn=true
warp_out_of_uncleared_minidungeons=true
```

## 构建

### 用 CMake + Visual Studio

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

### 直接运行脚本

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
```

输出：

```text
build-cl/ERDMod.dll
```

## 打包

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package_mod.ps1
```

输出目录：

```text
dist/mods/
```

其中会包含：
- `ERDMod.dll`
- `Config/ERDMod.ini`
- `Resources/...`

## 使用思路

1. 准备 Elden Mod Loader。
2. 只在离线环境测试。
3. 把 `dist/mods` 下的内容放进游戏目录下的 `mods` 文件夹。
4. 启动游戏并检查 `Logs/ERDMod.log`。

## 后续扩展建议

- 继续给 `Features` 增加新 CT 功能
- 增加热键绑定
- 给菜单加分页和功能说明
- 引入更多 hook 功能，而不只是内存写值
