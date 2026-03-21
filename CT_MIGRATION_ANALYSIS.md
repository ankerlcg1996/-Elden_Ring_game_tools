# Elden Ring CT Migration Analysis

针对 `eldenring_all-in-one_Hexinton-v5.0_ce7.5.ct` 和其拆分源码目录 `ct/` 的迁移分析。

## 结论

如果目标是把 CT 里的某个功能迁移成 DLL 注入插件，最适合的顺序是：

1. `NoDamage` / `NoStaminaConsumption` 这类纯指针链 + 位标志功能
2. `Custom FPS Limit`
3. `Noclip`
4. `Free Camera`
5. `Teleport to Map-Relative Coordinates`

原因不是“功能强弱”，而是迁移复杂度不同。

## 迁移类型

### 1. 纯地址链 / 位标志

特点：

- 不需要 AOB hook
- 不需要 Lua helper
- 只要能拿到单例指针，就能直接读写

代表：

- `Hero/Character Flags/NoDamage.xml`
- `Hero/Character Flags/NoDead.xml`
- `Hero/Character Flags/NoStaminaConsumption.xml`

例如 `NoDamage`：

- 基址：`WorldChrMan`
- 偏移链：`+10EF8 -> +0*10 -> +190 -> +0 -> +19B`
- 操作：改这个字节的 bit 1

这类功能最适合先做 DLL PoC，因为它们能最快验证：

- 单例解析是否正确
- 指针链是否正确
- 插件热键或配置切换是否生效

## 2. 纯字段改写

特点：

- 比位标志只高半级
- 需要知道管理器对象和固定偏移

代表：

- `Scripts/QoL/Custom FPS Limit/.cea`

脚本本体只做了两件事：

- 把 `[CSFlipper]+2CC` 写成 `float 75.0`
- 把后面的一个字节写成 `1`

这说明它本质上是：

- 找到 `CSFlipper`
- 写入 `debug_fps`
- 开启 `use_debug_fps`

这类功能也非常适合 DLL 化，因为逻辑简单，几乎不依赖 CE 特有机制。

## 3. 原生 hook 型功能

特点：

- 逻辑已经是 C 风格
- 最适合翻译成真正的 C++ DLL
- 但对版本更敏感

代表：

- `Scripts/Cheats/Noclip.cea`

这个脚本里已经有很完整的原生逻辑：

- 从 `FD4PadManager` 取输入
- 从 `FieldArea` 取视图矩阵
- 从 `CSFlipper` 取 delta time
- 从 `WorldChrMan` 识别本地玩家
- 在 `$process+467994` 打一个物理更新 hook

这类脚本迁移成 DLL 的收益很高，因为：

- CE 的 C block 已经接近原生插件代码
- 你只需要把 `alloc/jmp/dealloc` 改成你自己的 hook 框架
- 输入轮询、向量运算、位置写回逻辑都能直接复用

## 4. Lua + AOB 混合补丁

特点：

- 迁移可行
- 但需要把 Lua 扫描/补丁逻辑改写成 C++

代表：

- `Camera/Free Camera/.cea`

它做了两层事情：

1. 用 3 个 AOB 找到若干 call target，然后把目标函数改成 `mov al, 1; ret`
2. 再用一个 AOB 在 `8B 93 C8 00 00 00 85 D2` 位置插入跳转，联动 `GameMan + 0xBC4`

所以 Free Camera 不是一个“单点改值”功能，而是一组补丁协同工作。

适合 DLL，但不建议拿它做第一项迁移。

## 5. Lua helper 包装器

特点：

- 表面脚本很短
- 真正逻辑藏在别的 helper 里
- 迁移时要先补齐上下文

代表：

- `Coordinates & Teleport/Teleport to Map-Relative Coordinates/ Teleport.cea`

这个脚本本体只调用：

- `getMapRelativeCoords("TargetCoords")`
- `mapRelativeTeleport(tgtPos)`

而实际 warp 能力又依赖：

- `MiscWIP/Dependencies/Global Functions/Warp_code.cea`
- `CSLuaEventManager`
- `executeCodeEx(...)`

所以它不是“不复杂”，而是“复杂被藏起来了”。

如果一开始就迁这个，容易陷入追 helper 和隐式依赖。

## 单例解析来源

CT 里大量功能都不是自己 AOB 找 `WorldChrMan`、`GameMan`、`CSFlipper`，而是依赖：

- `MiscWIP/Dependencies/FD4Singleton Finder & Symbol Registerer.cea`

它的职责是：

- 找 FD4 单例
- 注册成 `WorldChrMan` / `GameMan` / `FieldArea` / `CSFlipper` 这类符号

如果你要做 DLL，第一步其实不是“移植某个 cheat”，而是：

1. 先做你自己的 singleton resolver
2. 再写一个 `GamePointers` 层把这些对象缓存起来
3. 最后再迁具体功能

## 推荐的 DLL 迁移顺序

### 路线 A：最快出成果

1. 先做 `WorldChrMan` 解析
2. 实现 `NoDamage`
3. 再实现 `NoStaminaConsumption`
4. 最后给这些开关加热键或配置

### 路线 B：先做一个“看得见”的功能

1. 先做 singleton resolver
2. 迁 `Custom FPS Limit`
3. 再迁 `Noclip`

### 路线 C：目标是做相机插件

1. 先做 pattern scanner
2. 再迁 `Free Camera`
3. 最后把 `Noclip` 合进去

## 我建议先搬哪个

如果你要的是“最稳、最像插件开发起点”的方案，我建议先搬：

- `NoDamage`

如果你要的是“最能证明 DLL 注入价值”的方案，我建议先搬：

- `Noclip`

如果你要的是“最终想做摄像机/拍照插件”，我建议先搬：

- `Custom FPS Limit`

因为它能先验证：

- singleton 解析
- 内存写入
- 配置化参数
- 版本兼容层

这些都是后面 `Free Camera` 必须用到的基础设施。
