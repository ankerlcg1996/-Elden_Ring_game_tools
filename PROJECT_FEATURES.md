# erdGameTools 功能树说明（以当前 DLL 为准）

> 产物基准：`build/Release/erdGameTools.dll`  
> 文档作用：记录“当前版本实际可用功能”，避免 README 与代码脱节。
> 当前形态：integrated-mode，所有功能直接内建于 `erdGameTools.dll`，不再扫描或依赖 `plugins\*.dll`。

## 1. 功能树（用户视角）

```text
赐福菜单
└─ erdGameTools
   ├─ 玩家修改
   │  ├─ 装备减重（0%~100%）
   │  ├─ 装备需求降低（0%~100%，20%步进）
   │  ├─ 玩家速度增加（0%~200%，10%步进）
   │  ├─ BUFF持续时间延长（0%/50%/100%/200%/300%/500%/永久）
   │  ├─ 开启武器弹刀（关闭 / 100%弹刀 / 简单 / 中等 / 大师）
   │  ├─ 调整玩家伤害倍率
   │  │  ├─ 增加（0%~500%，低段细分高段稀疏）
   │  │  └─ 减少[增加难度]（0%~500%，低段细分高段稀疏）
   │  ├─ 调整玩家减免伤害倍率
   │  │  ├─ 减免伤害（0%~500%，低段细分高段稀疏）
   │  │  └─ 增加受到伤害[增加难度]（0%~500%，低段细分高段稀疏）
   │  ├─ 精力消耗减少（0%~100%，10%步进）
   │  ├─ FP(魔法)消耗减少（0%~100%，10%步进）
   │  ├─ 魔法没有使用需求
   │  ├─ 所有魔法只占一个栏位
   │  └─ 复血机制（Rally）
   ├─ 游戏修改
   │  ├─ 商店免费购买
   │  ├─ 制作不消耗材料
   │  ├─ 强化不消耗材料
   │  ├─ 所有武器可附魔
   │  ├─ 所有武器可更换战灰
   │  ├─ 随处召唤骨灰
   │  ├─ 灵马不死
   │  ├─ 任意地点骑马
   │  ├─ 消耗品无限
   │  ├─ 箭矢无限
   │  ├─ 无限跳（骑马可用）
   │  ├─ 死亡不掉卢恩
   │  ├─ 死亡不掉卢恩弯弧
   │  ├─ 死亡不推进时间
   │  └─ 不会摔死
   ├─ 游戏周目(NG)修改
   │  └─ 当前周目切换（NG / NG+1 ~ NG+10）
   ├─ 可视化敌人抗性
   │  ├─ 显示锁定目标韧性条（BOSS/普通敌人统一固定在屏幕中下）
   │  ├─ 显示锁定目标异常条（普通敌人贴顶显示，BOSS 显示在原生血条下方）
   │  ├─ 菜单内可直接循环微调 HUD：韧性条/异常条宽高、上下偏移、BOSS 间距
   │  │  HUD微调
   │  │  韧性条宽度 / 韧性条高度
   │  │  异常条宽度 / 异常条高度
   │  │  韧性条上下偏移 / 普通敌人异常条上下偏移 / BOSS异常条间距
   │  ├─ 单独开关出血 / 冻伤 / 腐败 / 中毒 / 睡眠 / 发狂
   │  ├─ 一键开启全部条
   │  ├─ 一键关闭全部条
   │  ├─ HUD 位置与宽高支持通过 `erdGameTools.ini` 配置微调
   │  ├─ 支持通过 `target_status_debug_logging=true` 输出锁定目标诊断日志
   │  └─ 不再提供伤害飘字
   ├─ 敌人修改
   │  ├─ 增加敌人血量（0%~1000%，200%内20%步进，200%后50%步进）
   │  └─ 减少敌人血量（0%~1000%，200%内20%步进，200%后50%步进，和增加互斥）
   ├─ 复活BOSS
   │  ├─ 区域（第2级）
   │  ├─ BOSS名（第3级，按内置 BossRevives CSV 数据生成）
   │  └─ BOSS操作（第4级：复活BOSS / 传送到BOSS）
   ├─ 自动拾取
   │  ├─ 总开关
   │  └─ 设置
   │     ├─ 自动拾取材料
   │     ├─ 自动拾取物品
   │     ├─ 自动拾取尸体战利品
   │     ├─ 自动拾取掉落卢恩
   │     ├─ 战斗中自动拾取
   │     └─ 自动拾取范围（100% / 150% / 200% / 300% / 500%）
   ├─ 快捷入口
   │  ├─ 强化
   │  ├─ 灵魂调谐
   │  ├─ 重生
   │  ├─ 打开镜子
   │  └─ 使用原生菜单直接打开对应界面
   └─ 功能反馈
      └─ 菜单开关、百分比调整、解锁/随身赐福/BOSS操作执行成功后，会弹出 generic dialog 显示本次动作与结果状态
   

常驻功能（非菜单项）
├─ DX12 锁定目标 HUD（可在赐福菜单中关闭）
├─ 伤害弹字（基于攻击信息 hook 的 UI 飘字，仅显示伤害大于 0 的命中，可在赐福菜单中关闭）
├─ 不再显示锁定目标调试监听面板
├─ 显示隐形（哨兵火把，SpEffect 416，默认开启且不可关闭）
├─ 快速复活（角色加载后自动开启）
├─ 未通关小地牢可传送离开（角色加载后自动开启）
├─ 战斗可打开地图（角色加载后自动开启）
├─ 永久提灯（角色加载后自动开启）
└─ 玩家激活特效监控（仅进游戏后，在右上角文字显示 Buff名称/ID/剩余时间/持续时间/状态）
   （仅当配置 `erdGameTools.ini` 的 `[Debug] debug=true` 时显示）
```

## 2. 功能树（代码模块视角）

```text
runtime (src/grace_test_runtime.cpp)
├─ EzStateTalkCommand (Source/Features/EzStateTalkCommand.cpp/.hpp)
│  ├─ 抽取了通用 EzState TalkCommand 执行器
│  └─ 当前用于快捷入口中的强化 / 灵魂调谐 / 重生 / 打开镜子
├─ CharacterFlags (Source/Features/CharacterFlags.cpp)
│  ├─ 无死亡（内部状态位）
│  ├─ 无限 FP（内部状态位）
│  ├─ 消耗品无限（ChrDbgFlags.InfiniteGoods 位）
│  ├─ 箭矢无限（ChrDbgFlags.InfiniteArrows 位）
│  ├─ 死亡不掉卢恩（代码补丁：NoRuneLossOnDeath）
│  ├─ 消除脚步声（ChrDbgFlags.Silent 位）
│  └─ 灵马不死（灵马 ChrDataFlags.NoDeath 位）
├─ ParamPatches (Source/Features/ParamPatches.cpp)
│  ├─ 快速复活
│  ├─ 未通关小地牢可传送离开
│  ├─ 商店免费购买
│  ├─ 制作不消耗材料
│  ├─ 强化不消耗材料
│  ├─ 所有武器可附魔（EquipParamWeapon +0x106 统一写 255，可恢复）
│  ├─ 所有武器可更换战灰（EquipParamWeapon.gemMountType 全表写 1，可恢复）
│  ├─ 魔法需求归零
│  ├─ 魔法统一 1 槽
│  ├─ 装备减重（按百分比）
│  ├─ 装备需求降低（EquipParamWeapon 需求字段按百分比）
│  ├─ 玩家速度增加（ChrIns -> [0x190,0x28] -> +0x17C8 写入动画速度）
│  ├─ BUFF持续时间延长（SpEffectParam[330600].extendLifeRate 写值，并应用/移除 SpEffect 330600）
│  ├─ 开启武器弹刀（EquipParamWeapon 全表写 parryDamageLife=60/40/20/10，并置 enableParry 位）
│  ├─ 旧版“650~657 SpEffect 常驻吸血”路径已停用，Rally 逻辑改由 RallyHooks 接管
│  ├─ 敌人血量增减（NpcParam.hp 全表倍率写入，增减互斥）
│  ├─ 玩家增加伤害倍率（SpEffectParam[416] 五属性 AttackPowerRate）
│  ├─ 玩家减少伤害倍率[增加难度]（SpEffectParam[416] 五属性 AttackPowerRate，互斥）
│  ├─ 玩家减免伤害倍率（SpEffectParam[416] 五属性 DamageCutRate，按百分比减值）
│  ├─ 玩家增加伤害倍率[增加难度]（SpEffectParam[416] 五属性 DamageCutRate，互斥）
│  ├─ 精力消耗减少（SpEffectParam[416].consumeStaminaRate）
│  ├─ FP(魔法)消耗减少（Magic.mp/mp_charge + Goods.consumeMP + Behavior.heroPoint 按百分比重写）
│  ├─ 自定义 FOV / 自定义镜头距离
│  ├─ 随处召唤骨灰
│  ├─ 任意地点骑马（双 AOB 检查强制返回可骑）
│  ├─ 战斗可打开地图（OpenMap + CloseMap 双补丁）
│  ├─ 无限跳（3处注入：2处强制 AL=1 + 1处比较立即数改写）
│  ├─ 不会摔死（SpEffectParam[360400].cycleOccurrenceSpEffectId 改为 185，且 SpEffectParam[185].cycleOccurrenceSpEffectId 改为 184）
│  │  运行时会监听玩家 184 / 185 状态，若缺失则在角色已加载后自动补挂；并将 184 / 185 的 effectEndurance 设为 -1
│  ├─ 死亡不掉卢恩弯弧（AOB 定位后把条件跳转 `0x74` 改写为 `0xEB`）
│  ├─ 死亡不推进时间（AOB 注入 code cave，死亡结算时回写 `WorldAreaTimeImpl` 当前时间到 `GameMan.StoredTime`）
│  ├─ 掉宝倍率
│  ├─ 永久提灯
│  └─ 头盔隐藏
├─ AutoPickup (Source/Features/AutoPickup.cpp/.hpp + AutoPickupExecActionButtonWrapper.asm)
│  ├─ 迁移自 ErdTools-CPP 的 ActionButton 自动拾取思路
│  ├─ Hook ExecuteActionButtonParamProxy，对选中的 ActionButtonParam 直接放行自动执行
│  ├─ 通过 ActionButtonParam.radius / height / baseHeightOffset 放大拾取范围
│  └─ 当前支持材料 / 物品 / 尸体战利品 / 掉落卢恩 / 战斗中自动拾取 / 范围倍率
├─ SpEffectMonitor (Source/Features/SpEffectMonitor.cpp/.hpp)
│  ├─ 提取了玩家 SpEffect 应用/移除底层调用
│  ├─ 已接：驱兽火把效果（460，菜单开关）
│  ├─ 已接：哨兵火把效果（416，常驻开启，并强制 SpEffectParam[416].changeMagicSlot=3）
│  ├─ 已接：不会摔死效果（360400，若缺少 185 或 184 则重挂）
│  └─ 提取了激活 SpEffect 列表监控并供调试面板显示
├─ RallyHooks (Source/Features/RallyHooks.cpp/.hpp)
│  ├─ 参考 Named-Blade 的 Rally Redone 实现，Hook 游戏内部 rally update / hp change 两条函数
│  ├─ 使用角色模块内原生 `rally_potential / rally_cap / rally_timer / rally_regain`
│  ├─ 仅对本地主角生效，且要求玩家当前存在 `state_info=449` 的 Rally 类 SpEffect
│  ├─ 支持命中重置复血窗口、指数/线性衰减、战斗中治疗转为填充 Rally 条
│  └─ 配置走 `erdGameTools.ini` 的 `[RallyMod]`
├─ InspectorTools (Source/Features/InspectorTools.cpp/.hpp)
│  ├─ 角色/队伍/背包快照
│  ├─ 参数表读取与写入
│  └─ ParamScripts 外置执行
├─ TargetStatusService (Source/Features/TargetStatusService.cpp/.hpp)
│  ├─ 内置锁定目标读取逻辑
│  ├─ 通过本地主角 target handle + GetChrInsFromHandle 解析当前锁定目标
│  ├─ 若 GetChrInsFromHandle 失效，则回退到 ChrSet 池手动解引用句柄
│  ├─ 可选输出诊断日志：handle / chr 地址 / fallback 是否命中 / 各模块指针可用性
│  ├─ 提取 HP / MaxHP / Poise / MaxPoise / Sleep / Poison / Rot / Frost / Bleed / Madness / DeathBlight
│  └─ 将锁定目标快照持续写入 FeatureStatus，供赐福功能与 Overlay 复用
├─ Player Read-Only Snapshot（src/grace_test_runtime.cpp -> FeatureStatus）
│  ├─ 读取玩家当前属性：等级 / 卢恩 / 记忆卢恩 / 生命力 / 集中力 / 耐力 / 力气 / 灵巧 / 智力 / 信仰 / 感应
│  ├─ 读取玩家当前状态：HP / FP / SP、幽影树赐福、灵灰赐福
│  ├─ 读取并写入当前周目：`GameDataMan.PlayerGameData + 0x120`
│  ├─ 角色成功进游戏后自动检查背包，若缺少 `2070 / 130 / 181 / 250 / 251` 则自动补发
│  ├─ 读取装备栏 ID：左右手武器 1/2/3、箭矢 1/2/3、弩箭 1/2/3、护符 1~5
│  ├─ 读取当前持手模式与左右手当前武器槽位（只读，不修改）
│  └─ 读取当前玩家模型大小三轴：左右宽度 / 高度 / 前后宽度
├─ 玩家模型修改（src/grace_test_runtime.cpp）
│  ├─ 按 CT 原始链写入 `[[[[WorldChrMan]+10EF8]+0*10]+58] + 0x2D4 / 0x2D8 / 0x2DC`
│  ├─ 三个字段均为 `float`
│  └─ 菜单选择单一百分比后，将三个维度统一写入相同值
├─ Boss Revive 数据与执行（src/grace_test_runtime.cpp）
│  ├─ 按语言加载 Resources/BossRevives-<lang>.csv（回退 en-US，再回退 BossRevives.csv）
│  ├─ 当前已提供 BossRevives-en-US.csv / BossRevives-zh-CN.csv
│  ├─ 按菜单ID索引 BOSS（区域/名称）
│  ├─ 复活时写 BossFlags 事件位
│  └─ 对未预设 InitializeDead 的条目，写 NpcParam 初始化死亡位（0x14D bit3）
└─ Overlay (src/grace_test_overlay.cpp)
   ├─ 使用 DX12 Hook（Present / ResizeBuffers / ExecuteCommandLists）在游戏帧内绘制
   ├─ 绘制锁定目标 HP / Poise 与可独立开关的异常抗性条
   ├─ 韧性条固定在屏幕中下，普通敌人异常条贴顶，BOSS 异常条挂在原生 BOSS 血条下方
   └─ 韧性/异常条宽高与垂直偏移可通过 `erdGameTools.ini` 调整

## 3. 分发与调试补充

```text
CMakeLists.txt / build_dist.ps1
├─ 编译 Release 版 erdGameTools.dll
├─ 复制 erdGameTools.ini
├─ 复制完整 Resources/*
└─ 输出 dist/erdGameTools/
```

```text
erdGameTools.ini
├─ [GameMods] 功能开关与倍率
├─ [Overlay] 敌人 HUD 尺寸 / 偏移 / 诊断日志
├─ [RallyMod] Rally 参数
└─ [Debug] 调试总开关
```

`erdGameTools.ini` 支持运行时热重载；手动修改 Overlay 配置后，无需重新编译 DLL。
   ├─ 直接读取当前锁定目标句柄，不再依赖敌人/BOSS 血条是否显示
   ├─ 菜单支持单独开关 Poise / Bleed / Frost / Rot / Poison / Sleep / Madness
   ├─ 支持一键全开与一键全关全部条
   ├─ 韧性主条使用 Green.png 作为主色贴图
   └─ 在 HUD 上显示精确数值（current / max）
├─ Integrated Feature Gate (src/grace_test_plugins.cpp/.hpp + src/erd_plugin_api.hpp)
│  └─ 兼容保留 feature group 枚举，但不再扫描或加载外部功能 DLL
├─ Overlay Host (src/grace_test_overlay.cpp/.hpp)
│  ├─ 主 DLL 内直接托管 DX12 overlay 生命周期
│  └─ 主 DLL 内直接维护 TargetStatusService
```

## 3. 已移除功能（当前版本不再提供）

- 无伤模式
- 道具不消耗
- 一键解锁（整组解锁功能）
- 架势条显示
- 不耗精力
- 旧版“650~657 SpEffect 常驻吸血”实现
- 旧版非 DX Hook 的贴附式透明 SpEffect 调试窗

## 4. AOB 兼容说明

- 关键扫描点已改为“多候选特征回退匹配”（当前已用于 `InspectorTools` 与 `ParamPatches` 的部分功能）。
- 新增版本支持时，优先在对应候选数组追加新特征，不直接覆盖旧特征。

## 5. 维护规则（强制）

- 每次新增、删除、重命名功能，必须同步更新本文件。
- 若代码与本文冲突，以“先修正文档再提交代码”为准。
