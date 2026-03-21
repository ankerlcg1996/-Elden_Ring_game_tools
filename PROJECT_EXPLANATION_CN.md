# 项目代码说明

这份文档对应当前 `ERDMod` 工程的自有源码说明，不包含第三方库 `ImGui` 和 `MinHook`。

## 总体流程

1. `DllMain` 在 `DLL_PROCESS_ATTACH` 时创建工作线程。
2. `MainThread()` 初始化路径、读取 `Config/ERDMod.ini`、启动日志。
3. `Runtime` 把配置写入 `g_FeatureStatus`，并安装 DX12 Overlay。
4. `SingletonRegistry` 扫描游戏主模块，恢复 FD4 单例。
5. 各 `Features::*` 模块在主循环中读取 `g_FeatureStatus`，把功能写入游戏内存。
6. `D3D12Overlay` 在 `Present` 阶段绘制 ImGui 菜单，并允许点击切换功能。

## 目录职责

| 路径 | 作用 | 说明 |
|---|---|---|
| `Source/Common.hpp` | 公共基础定义 | 全局模块句柄、运行状态、轮询间隔、常用头文件 |
| `Source/dllmain.cpp` | DLL 入口 | 只做最小工作，避免在 `DllMain` 里执行重逻辑 |
| `Source/ERDMod.cpp` | 主线程入口 | 初始化路径、配置、日志、Runtime、Overlay |
| `Source/Main/Config.*` | 配置系统 | 解析 ini，支持节名配置和旧版平铺键名 |
| `Source/Main/Paths.*` | 路径系统 | 统一用 DLL 相对路径定位配置、日志、资源 |
| `Source/Main/Logger.*` | 日志系统 | 输出到调试器和可选日志文件 |
| `Source/Main/FeatureStatus.hpp` | 运行时状态 | 菜单、功能模块、日志共享的开关状态 |
| `Source/Main/Runtime.*` | 主循环调度 | 串联配置初始化、单例解析、功能 Tick |
| `Source/Main/D3D12Overlay.*` | 游戏内菜单 | Hook DX12，绘制可点击 ImGui 界面 |
| `Source/Game/Memory.hpp` | 内存读写 | 提供安全读写、多级指针链和 bit flag 写入 |
| `Source/Game/SingletonRegistry.*` | FD4 单例解析 | 从游戏中恢复 `WorldChrMan`、`CSRegulationManager` 等 |
| `Source/Game/EventFlags.*` | 事件标志 | 直接写 Event Flag，支撑地图/食谱/赐福等解锁 |
| `Source/Game/Params.hpp` | 参数表访问 | 定位 Param 行，支撑参数补丁型功能 |
| `Source/Features/CharacterFlags.*` | 持续型 bit 开关 | 例如无限 FP、无限道具消耗 |
| `Source/Features/Unlocks.*` | 一次性解锁 | 例如解锁地图、食谱、砥石刀 |
| `Source/Features/ParamPatches.*` | 一次性参数补丁 | 例如更快复活、小地牢强制传出 |
| `scripts/build.ps1` | 一键编译 | 直接调用 MSVC `cl` 出 DLL |
| `scripts/package_mod.ps1` | 一键打包 | 把 DLL、配置、资源整理到 `dist/mods` |

## 当前已实现功能

| 功能 | 模块 | 实现方式 | 当前菜单形态 |
|---|---|---|---|
| Infinite FP | `CharacterFlags` | 写 `WorldChrMan` 角色标志位 | 复选框 |
| Infinite Item Use | `CharacterFlags` | 写 `WorldChrMan` 道具消耗标志位 | 复选框 |
| Unlock All Maps | `Unlocks` | 批量写 Event Flag | 一次性按钮 |
| Unlock All Cookbooks | `Unlocks` | 批量写 Event Flag | 一次性按钮 |
| Unlock All Whetblades | `Unlocks` | 批量写 Event Flag | 一次性按钮 |
| Faster Respawn | `ParamPatches` | 修改 `MenuCommonParam` 指定行 | 一次性按钮 |
| Warp Out Of Uncleared Mini-Dungeons | `ParamPatches` | 修改 `SpEffectParam` 指定行 | 一次性按钮 |

## 为什么菜单分成“复选框”和“按钮”

| 类型 | 原因 |
|---|---|
| 复选框 | 对应持续生效的 bit flag，可以在主循环里反复写回，逻辑上可开可关 |
| 一次性按钮 | 对应事件标志或参数表补丁，当前版本只做“应用”不做“回滚” |

## 后续扩展时的判断标准

| CT 功能类型 | 推荐落点 |
|---|---|
| 指针链 + bit flag | `Source/Features/CharacterFlags.*` |
| 批量事件标志解锁 | `Source/Features/Unlocks.*` |
| Param 行修改 | `Source/Features/ParamPatches.*` |
| 真正的实时玩法改动、输入逻辑、相机/穿墙 | 需要新增 Hook 模块，不能只靠当前三类特性 |

## 维护建议

- 新功能先判断它属于“持续写值”“一次性解锁”还是“真正 hook”。
- 能用 `SingletonRegistry + Memory.hpp` 解决的，优先不要上 hook。
- 任何带 AOB 注入、函数跳板、输入重写的功能，都建议单独建新模块，不要混进简单的 `Tick()` 写值里。
- `ImGui` 和 `MinHook` 是第三方代码，不建议手改；要改行为尽量改我们自己的 `Source/Main/D3D12Overlay.*`。
