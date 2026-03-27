# erdGameTools Mod Features / 功能说明

> This document is a concise bilingual feature overview for the current build.  
> 本文档是当前版本的双语精简功能说明。

## English

### 1. In-Game Native Menu

The mod injects `erdGameTools` into the Site of Grace menu (no separate trainer window).

- `Player Mods`
- `Game Mods`
- `Enemy Mods`
- `Boss Revive`

### 2. Player Mods

- Equipment Weight Reduction (`0%~100%`)
- Equipment Requirement Reduction (`0%~100%`)
- Player Speed Boost (`0%~200%`)
- Buff Duration Extension (`0/50/100/200/300/500/Permanent`)
- Player Damage Increase (`0%~500%`)
- Player Damage Reduction Taken (`0%~500%`)
- Difficulty-side inverse damage options (`0%~500%`, mutually exclusive where applicable)
- No Magic Requirements
- All Magic Uses One Slot
- Infinite Consumables / Arrows / Stamina / FP
- No Rune Loss on Death

### 3. Game Mods

- Free Shop Purchase
- No Craft Material Cost
- No Upgrade Material Cost
- All Weapons Can Be Buffed
- All Weapons Can Change Ash of War
- Summon Spirit Ashes Anywhere
- Torrent Never Dies
- Ride Anywhere

### 4. Enemy Mods

- Increase Enemy HP (`0%~1000%`)
- Decrease Enemy HP (`0%~1000%`, mutually exclusive with increase)

### 5. Boss Revive

- Region menu -> Boss menu
- Per boss action menu:
- Revive Boss
- Warp to Boss

### 6. Always-On / Runtime Features

- Damage popup rendering (always enabled)
- Invisible reveal effect (`SpEffect 416`) auto-maintained after player load
- Fast revive / open map in combat / dungeon leave checks / perpetual lantern (auto-enabled on load)
- Active `SpEffect` monitor overlay (top-right) with `Name + ID + Left + Duration + Category + State`
- Overlay is shown only when `erdGameTools.ini` -> `[Debug] debug=true`

### 7. Resource & Config Files

- Config: `erdGameTools.ini`
- Localization: `Resources/Lang/en-US.txt`, `Resources/Lang/zh-CN.txt`
- Boss revive data: `Resources/BossRevives-en-US.csv`, `Resources/BossRevives-zh-CN.csv`
- SpEffect names: `Resources/SpEffectParam.txt`

---

## 简体中文

### 1. 原生游戏内菜单

模组会把 `erdGameTools` 注入赐福菜单，不需要额外训练器窗口。

- `玩家修改`
- `游戏修改`
- `敌人修改`
- `复活BOSS`

### 2. 玩家修改

- 装备减重（`0%~100%`）
- 装备需求降低（`0%~100%`）
- 玩家速度增加（`0%~200%`）
- BUFF 持续时间延长（`0/50/100/200/300/500/永久`）
- 玩家增加伤害倍率（`0%~500%`）
- 玩家减免伤害倍率（`0%~500%`）
- 难度向的反向伤害倍率功能（`0%~500%`，部分互斥）
- 魔法没有使用需求
- 所有魔法只占一个栏位
- 消耗品无限 / 箭矢无限 / 耐力无限 / FP无限
- 死亡不掉卢恩

### 3. 游戏修改

- 商店免费购买
- 制作不消耗材料
- 强化不消耗材料
- 所有武器可附魔
- 所有武器可更换战灰
- 随处召唤骨灰
- 灵马不死
- 任意地点骑马

### 4. 敌人修改

- 增加敌人血量（`0%~1000%`）
- 减少敌人血量（`0%~1000%`，与增加互斥）

### 5. 复活BOSS

- 区域菜单 -> BOSS菜单
- 选择具体 BOSS 后可执行：
- 复活BOSS
- 传送到BOSS

### 6. 常驻 / 运行时功能

- 伤害弹字常驻开启
- 显示隐形效果（`SpEffect 416`）在角色加载后自动维持
- 快速复活、战斗开图、小地牢传送离开、永久提灯等为角色加载自动启用
- 右上角玩家 SpEffect 监控显示：`名称 + ID + 剩余时间 + 持续时间 + 分类 + 状态`
- 仅当 `erdGameTools.ini` 的 `[Debug] debug=true` 时显示该监控

### 7. 资源与配置文件

- 配置文件：`erdGameTools.ini`
- 语言文件：`Resources/Lang/en-US.txt`、`Resources/Lang/zh-CN.txt`
- BOSS复活数据：`Resources/BossRevives-en-US.csv`、`Resources/BossRevives-zh-CN.csv`
- SpEffect 名称库：`Resources/SpEffectParam.txt`
