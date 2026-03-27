# erdGameTools 功能树说明（以当前 DLL 为准）

> 产物基准：`build/Release/erdGameTools.dll`  
> 文档作用：记录“当前版本实际可用功能”，避免 README 与代码脱节。

## 1. 功能树（用户视角）

```text
赐福菜单
└─ erdGameTools
   ├─ 玩家修改
   │  ├─ 装备减重（0%~100%）
   │  ├─ 装备需求降低（0%~100%，20%步进）
   │  ├─ 玩家速度增加（0%~200%，10%步进）
   │  ├─ BUFF持续时间延长（0%/50%/100%/200%/300%/500%/永久）
   │  ├─ 玩家增加伤害倍率（0%~500%，低段细分高段稀疏）
   │  ├─ 玩家减免伤害倍率（0%~500%，低段细分高段稀疏）
   │  ├─ 玩家减少伤害倍率[增加难度]（0%~500%，低段细分高段稀疏）
   │  ├─ 玩家增加伤害倍率[增加难度]（0%~500%，低段细分高段稀疏）
   │  ├─ 魔法没有使用需求
   │  ├─ 所有魔法只占一个栏位
   │  ├─ 消耗品无限
   │  ├─ 箭矢无限
   │  ├─ 耐力无限
   │  ├─ FP无限
   │  └─ 死亡不掉卢恩
   ├─ 游戏修改
   │  ├─ 商店免费购买
   │  ├─ 制作不消耗材料
   │  ├─ 强化不消耗材料
   │  ├─ 所有武器可附魔
   │  ├─ 所有武器可更换战灰
   │  ├─ 随处召唤骨灰
   │  ├─ 灵马不死
   │  ├─ 任意地点骑马
   ├─ 敌人修改
   │  ├─ 增加敌人血量（0%~1000%，200%内20%步进，200%后50%步进）
   │  └─ 减少敌人血量（0%~1000%，200%内20%步进，200%后50%步进，和增加互斥）
   ├─ 复活BOSS
   │  ├─ 区域（第2级）
   │  ├─ BOSS名（第3级，按 TarnishedTool-CN BossRevives 数据生成）
   │  └─ BOSS操作（第4级：复活BOSS / 传送到BOSS）
   

常驻功能（非菜单项）
├─ 伤害弹字（默认开启，不可关闭）
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
├─ CharacterFlags (Source/Features/CharacterFlags.cpp)
│  ├─ 无死亡（内部状态位）
│  ├─ 无限 FP（内部状态位）
│  ├─ 消耗品无限（ChrDbgFlags.InfiniteGoods 位）
│  ├─ 箭矢无限（ChrDbgFlags.InfiniteArrows 位）
│  ├─ 耐力无限（ChrDbgFlags.InfiniteStam 位）
│  ├─ FP无限（ChrDbgFlags.InfiniteFp 位）
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
│  ├─ 敌人血量增减（NpcParam.hp 全表倍率写入，增减互斥）
│  ├─ 玩家增加伤害倍率（SpEffectParam[416] 五属性 AttackPowerRate）
│  ├─ 玩家减少伤害倍率[增加难度]（SpEffectParam[416] 五属性 AttackPowerRate，互斥）
│  ├─ 玩家减免伤害倍率（SpEffectParam[416] 五属性 DamageCutRate，按百分比减值）
│  ├─ 玩家增加伤害倍率[增加难度]（SpEffectParam[416] 五属性 DamageCutRate，互斥）
│  ├─ 更容易弹反（按百分比，来自 AA 注入）
│  ├─ 自定义 FOV / 自定义镜头距离
│  ├─ 随处召唤骨灰
│  ├─ 任意地点骑马（双 AOB 检查强制返回可骑）
│  ├─ 战斗可打开地图（OpenMap + CloseMap 双补丁）
│  ├─ 无限跳（3处注入：2处强制 AL=1 + 1处比较立即数改写）
│  ├─ 掉宝倍率
│  ├─ 永久提灯
│  └─ 头盔隐藏
├─ SpEffectMonitor (Source/Features/SpEffectMonitor.cpp/.hpp)
│  ├─ 提取了玩家 SpEffect 应用/移除底层调用
│  ├─ 已接：驱兽火把效果（460，菜单开关）
│  ├─ 已接：哨兵火把效果（416，常驻开启）
│  └─ 提取了激活 SpEffect 列表监控并供 ImGui 绘制
├─ InspectorTools (Source/Features/InspectorTools.cpp/.hpp)
│  ├─ 角色/队伍/背包快照
│  ├─ 参数表读取与写入
│  └─ ParamScripts 外置执行
├─ Boss Revive 数据与执行（src/grace_test_runtime.cpp）
│  ├─ 按语言加载 Resources/BossRevives-<lang>.csv（回退 en-US，再回退 BossRevives.csv）
│  ├─ 当前已提供 BossRevives-en-US.csv / BossRevives-zh-CN.csv
│  ├─ 按菜单ID索引 BOSS（区域/名称）
│  ├─ 复活时写 BossFlags 事件位
│  └─ 对未预设 InitializeDead 的条目，写 NpcParam 初始化死亡位（0x14D bit3）
└─ Overlay (src/grace_test_overlay.cpp)
   ├─ DX12 Hook 与伤害弹字渲染链路
   └─ 启动时加载 `Resources/SpEffectParam.txt` 到内存映射，并在 SpEffect 监控中按 ID 显示 Buff 名称
```

## 3. 已移除功能（当前版本不再提供）

- 无伤模式
- 道具不消耗
- 一键解锁（整组解锁功能）
- 架势条显示
- 快捷入口
- 不耗精力

## 4. AOB 兼容说明

- 关键扫描点已改为“多候选特征回退匹配”（当前已用于 `InspectorTools` 与 `ParamPatches` 的部分功能）。
- 新增版本支持时，优先在对应候选数组追加新特征，不直接覆盖旧特征。

## 5. 维护规则（强制）

- 每次新增、删除、重命名功能，必须同步更新本文件。
- 若代码与本文冲突，以“先修正文档再提交代码”为准。
