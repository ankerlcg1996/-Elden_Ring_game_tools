# CT 功能整理表

基于本地 `eldenring_all-in-one_Hexinton-v5.0_ce7.5.ct` 目录结构整理。  
这份表不是逐条翻译 8000+ 个 XML/CEA 叶子节点，而是按“实际功能域”做细致归类，方便你决定下一批适合搬进 DLL 的内容。

## 顶层模块统计

| 顶层目录 | 条目数（xml/cea） | 说明 |
|---|---:|---|
| `Camera` | 107 | 相机参数、自由相机、调试相机 |
| `Coordinates & Teleport` | 45 | 坐标查看、坐标传送、血迹传送 |
| `Debug Stuff` | 252 | 游戏内部调试信息和图形/系统调试 |
| `Event Flags` | 974 | 事件标志管理、按 ID 查看和批量解锁 |
| `Hero` | 773 | 玩家角色数据、装备、状态、动作、外观 |
| `MiscWIP` | 98 | 依赖脚本、下拉表、实验性内容 |
| `Param Mods` | 753 | 参数表浏览、查询、编辑辅助 |
| `Scripts` | 5308 | 真正的脚本功能、帮助器、建号、QOL、NPC 菜单 |
| `Statistics` | 293 | 角色信息、会话信息、存档控制 |
| `Table Tools` | 15 | CT 自身维护工具 |

## 功能矩阵

| 分类 | 代表 CT 路径 | 代表功能 | 中文说明 | DLL 实现方式 | 适合度 | 建议 |
|---|---|---|---|---|---|---|
| 角色标志 | `Hero/Character Flags` | `NoDamage` | 无伤 | 指针链 + bit flag | 高 | 很适合添加 |
| 角色标志 | `Hero/Character Flags` | `NoDead` | 不死 | 指针链 + bit flag | 高 | 很适合添加 |
| 角色标志 | `Hero/Character Flags` | `NoFPConsumption` | 不耗 FP / 无限蓝耗 | 指针链 + bit flag | 已实现 | 已在项目中 |
| 角色标志 | `Hero/Character Flags` | `NoGoodsConsume` | 道具不消耗 | 指针链 + bit flag | 已实现 | 已在项目中 |
| 角色标志 | `Hero/Character Flags` | `NoStaminaConsumption` | 不耗精力 | 指针链 + bit flag | 高 | 很适合添加 |
| 角色标志 | `Hero/Character Flags` | `No Attack / No Hit / No Move / No Update` | 关闭攻击、命中、移动或角色更新 | 指针链 + bit flag | 中 | 能做，但误操作风险大 |
| QOL 参数补丁 | `Scripts/QoL` | `Faster respawn` | 缩短死亡到复活等待时间 | Param 修改 | 已实现 | 已在项目中 |
| QOL 参数补丁 | `Scripts/QoL` | `Warp out of uncleared mini-dungeons` | 未清小地牢可强制传出 | Param / SpEffect 修改 | 已实现 | 已在项目中 |
| QOL 参数补丁 | `Scripts/QoL` | `Custom FOV` | 自定义视野角 | Param 或相机结构写值 | 高 | 很适合添加 |
| QOL 参数补丁 | `Scripts/QoL` | `Custom Camera Distance` | 自定义镜头距离 | 相机结构写值 | 高 | 很适合添加 |
| QOL 参数补丁 | `Scripts/QoL` | `Custom FPS Limit` | 自定义帧率上限 | 写全局结构 / 参数 | 高 | 很适合添加 |
| QOL 参数补丁 | `Scripts/QoL` | `Boost for low FPS` | 低帧率优化 | 多项参数补丁 | 中 | 可做，但需核对版本 |
| QOL 参数补丁 | `Scripts/QoL` | `Item discovery x10` | 掉宝率倍率 | Param 或状态值修改 | 中高 | 值得加 |
| QOL 参数补丁 | `Scripts/QoL` | `Permanent Lantern Toggle` | 常亮提灯 | 状态位或 SpEffect | 中高 | 值得加 |
| QOL 参数补丁 | `Scripts/QoL` | `Invisible helmets` | 头盔隐藏 | 外观/装备显示位 | 中 | 可做 |
| QOL 参数补丁 | `Scripts/QoL` | `Discardable Items` | 某些不可丢弃物变为可丢弃 | Param 修改 | 中 | 风险中等 |
| QOL 参数补丁 | `Scripts/QoL` | `World Map Cursor Speed` | 世界地图光标加速 | UI / 参数写值 | 中 | 可做 |
| 建号/资源 | `Scripts/Build Creation/AddSoul` | `Give Runes` | 直接增加卢恩 | 调用内部函数或写数值 | 中高 | 值得加 |
| 建号/资源 | `Scripts/Build Creation/ItemGib` | `Spawn Item` | 发物品、数量、强化等级、战灰 | 调内部函数 | 中 | 适合做成独立菜单 |
| 建号/资源 | `Scripts/Build Creation` | `Unlock All Gestures` | 解锁全部手势 | Event Flag / 内部函数 | 高 | 很适合添加 |
| 建号/资源 | `Scripts/Build Creation` | `Add charge to flask / Set flask level` | 修改圣杯瓶数量和等级 | 角色数据或内部函数 | 中高 | 值得加 |
| 建号/资源 | `Scripts/Build Creation` | `Upgrades Need No Materials` | 强化不耗材料 | Param 或逻辑位修改 | 高 | 很适合添加 |
| 解锁 | `Event Flags/Unlock all` | `Unlock all Maps` | 解锁全部地图 | 批量 Event Flag | 已实现 | 已在项目中 |
| 解锁 | `Event Flags/Unlock all` | `Unlock all Cookbooks` | 解锁全部食谱 | 批量 Event Flag | 已实现 | 已在项目中 |
| 解锁 | `Event Flags/Unlock all` | `Unlock all Whetblades` | 解锁全部砥石刀 | 批量 Event Flag | 已实现 | 已在项目中 |
| 解锁 | `Event Flags/Unlock all` | `Unlock all Graces` | 解锁全部赐福 | 批量 Event Flag | 高 | 很适合添加 |
| 解锁 | `Event Flags/Unlock all` | `Unlock all Summoning Pools` | 解锁全部召唤池 | 批量 Event Flag | 高 | 很适合添加 |
| 解锁 | `Event Flags/Unlock all` | `Unlock all Colosseums` | 解锁全部斗技场 | 批量 Event Flag | 高 | 很适合添加 |
| 事件标志工具 | `Event Flags/Event Flags by ID` | `Event Flag ID / State / Category` | 按 ID 查看和切换 Flag | Flag 查询 + 菜单输入 | 中高 | 很适合做调试页 |
| 坐标/传送 | `Coordinates & Teleport` | `Player Character Local/Global Coords` | 查看本地/全局坐标 | 读坐标结构 | 高 | 很适合添加 |
| 坐标/传送 | `Coordinates & Teleport` | `Teleport to Map-Relative Coordinates` | 传送到指定坐标 | 写坐标 + 调内部 warp | 中 | 要谨慎处理状态同步 |
| 坐标/传送 | `Coordinates & Teleport/Bloodstain` | 血迹传送 | 传送到血迹点 | 依赖更多上下文 | 中低 | 后做 |
| 相机 | `Camera/Free Camera` | `Free camera mode [A+L3]` | 自由相机 | AOB + 输入逻辑 + 相机 hook | 中低 | 适合单独大功能 |
| 相机 | `Camera/CSPersCam*` | `FOV / Render Dist / XYZ` | 第三人称相机参数 | 相机结构写值 | 中高 | 可先做简版 |
| 脚本作弊 | `Scripts/Cheats` | `Noclip` | 穿墙、自由移动 | 真正 hook + 输入 + 位置更新 | 中低 | 很强，但工作量大 |
| 脚本作弊 | `Scripts/Cheats` | `Kill all mobs` | 清怪 | 遍历 NPC / 调伤害逻辑 | 低到中 | 风险较高 |
| 统计/存档 | `Statistics/Character` | `Health / Focus / Stamina / Level` | 查看角色基础数据 | 只读或直接写值 | 中高 | 适合做调试面板 |
| 统计/存档 | `Statistics/Misc` | `Start Next Cycle (NG+)` | 开始下一周目 | 调内部函数 | 中 | 高风险操作，需确认 |
| 统计/存档 | `Statistics/SaveLoad2` | `Save Request / Reload Character / Load Save Slot` | 请求保存、重载角色、切槽 | 调内部函数 | 低到中 | 风险较高 |
| NPC 菜单 | `Scripts/Npc Menus` | `Shops / Rennala / Site of Grace` | 强制打开商店、洗点、赐福菜单 | 调内部 UI / 菜单函数 | 中 | 适合后续做高级功能 |
| Seamless 修复 | `Scripts/Seamless Co-op` | `Loading screen stuck fix / Ranni's Tower Fix` | 联机扩展场景修补 | 针对特定 Mod 的补丁 | 低 | 不建议优先做 |
| Param 工具 | `Param Mods/Tools` | `Find address by ID / Print param list` | 参数表查询与调试工具 | 内部开发工具 | 中 | 适合做开发者页 |
| Helpers | `Scripts/Helpers` | `Targeted Npc Info / Last Hit Npc Info` | 查看目标 NPC 或最后命中 NPC 的数据 | 读结构体并展示 | 中高 | 很适合做调试页 |
| Table Tools | `Table Tools` | `Configure / Unpack / Clear Symbol Cache` | 维护 CT 自身，不是游戏功能 | 不需要搬进 DLL | 低 | 不建议添加 |

## 最适合下一批添加的功能

| 优先级 | 功能 | 原因 |
|---|---|---|
| P1 | `NoStaminaConsumption` | 与现有 `Infinite FP / Infinite Items` 同类，改动最小 |
| P1 | `NoDamage` | 也是纯 bit flag，落地快，玩家感知强 |
| P1 | `Unlock all Graces` | 与现有解锁模块完全同类，迁移成本低 |
| P1 | `Unlock all Summoning Pools` | 同上，适合放到现有一次性按钮页 |
| P1 | `Unlock All Gestures` | 属于一次性解锁，菜单体验好 |
| P2 | `Custom FOV` | 用户体验明显，且比自由相机简单得多 |
| P2 | `Custom Camera Distance` | 与 FOV 同类，适合放在相机页 |
| P2 | `Give Runes` | 需求常见，适合做输入框 + 按钮 |
| P2 | `Flask charge / level` | 需求明确，适合做角色资源页 |
| P2 | `Targeted Npc Info` | 很适合做调试菜单，方便后续开发更复杂功能 |

## 不建议马上添加的功能

| 功能 | 原因 |
|---|---|
| `Free Camera` | 涉及输入逻辑、相机 hook、状态恢复，工作量明显高于普通写值功能 |
| `Noclip` | 需要持续接管移动/碰撞逻辑，版本敏感度高 |
| `Kill all mobs` | 风险高，容易误伤友军/脚本目标，最好先把 NPC 查询页做出来 |
| `Load Save Slot / Reload Character / NG+` | 影响存档或流程，误触代价高 |
| `Seamless Co-op fixes` | 强依赖其他 Mod 的上下文，不适合作为通用基础功能 |

## 适合下一版菜单增加的分页

| 页面 | 推荐内容 |
|---|---|
| `Player` | Infinite HP、FP、Stamina、No Damage、No Death、Flask 设置 |
| `Unlocks` | Maps、Graces、Cookbooks、Whetblades、Summoning Pools、Gestures |
| `World` | 坐标显示、快速传送、目标赐福、地图光标速度 |
| `Camera` | FOV、镜头距离、渲染距离 |
| `Debug` | Targeted NPC Info、Last Hit NPC Info、Event Flag by ID、Param 查询 |

## 结论

如果按“最快见效 + 最低风险 + 最贴合现有架构”来选，建议下一批优先做：

1. `NoStaminaConsumption`
2. `NoDamage`
3. `Unlock all Graces`
4. `Unlock all Summoning Pools`
5. `Unlock All Gestures`
6. `Custom FOV`
7. `Custom Camera Distance`
