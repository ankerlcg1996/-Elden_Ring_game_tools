# erdGameTools 功能树说明（以当前 DLL 为准）

> 产物基准：`build/Release/erdGameTools.dll`  
> 文档作用：记录“当前版本实际可用功能”，避免 README 与代码脱节。

## 1. 功能树（用户视角）

```text
赐福菜单
└─ erdGameTools
   ├─ 玩家修改
   │  ├─ 装备减重（10%~100%）
   │  ├─ 装备需求降低（20%~100%）
   │  ├─ 魔法没有使用需求
   │  ├─ 所有魔法只占一个栏位
   │  ├─ 消除脚步声
   │  └─ 更容易弹反（10%~100%）
   └─ 游戏修改
      ├─ 商店免费购买
      ├─ 制作不消耗材料
      ├─ 强化不消耗材料
      ├─ 所有武器可附魔
      ├─ 随处召唤骨灰
      ├─ 灵马不死
      └─ 任意地点骑马

常驻功能（非菜单项）
├─ 伤害弹字（默认开启，不可关闭）
└─ 玩家激活特效监控（仅进游戏后，在右上角文字显示 ID/剩余时间/持续时间/状态）
```

## 2. 功能树（代码模块视角）

```text
runtime (src/grace_test_runtime.cpp)
├─ CharacterFlags (Source/Features/CharacterFlags.cpp)
│  ├─ 无死亡（内部状态位）
│  ├─ 无限 FP（内部状态位）
│  ├─ 消除脚步声（ChrDbgFlags.Silent 位）
│  └─ 灵马不死（灵马 ChrDataFlags.NoDeath 位）
├─ ParamPatches (Source/Features/ParamPatches.cpp)
│  ├─ 快速复活
│  ├─ 未通关小地牢可传送离开
│  ├─ 商店免费购买
│  ├─ 制作不消耗材料
│  ├─ 强化不消耗材料
│  ├─ 所有武器可附魔（EquipParamWeapon +0x106 统一写 255，可恢复）
│  ├─ 魔法需求归零
│  ├─ 魔法统一 1 槽
│  ├─ 装备减重（按百分比）
│  ├─ 装备需求降低（EquipParamWeapon 需求字段按百分比）
│  ├─ 更容易弹反（按百分比，来自 AA 注入）
│  ├─ 自定义 FOV / 自定义镜头距离
│  ├─ 随处召唤骨灰
│  ├─ 任意地点骑马（双 AOB 检查强制返回可骑）
│  ├─ 掉宝倍率
│  ├─ 永久提灯
│  └─ 头盔隐藏
├─ SpEffectMonitor (Source/Features/SpEffectMonitor.cpp/.hpp)
│  ├─ 提取了玩家 SpEffect 应用/移除底层调用（暂不接菜单）
│  └─ 提取了激活 SpEffect 列表监控并供 ImGui 绘制
├─ InspectorTools (Source/Features/InspectorTools.cpp/.hpp)
│  ├─ 角色/队伍/背包快照
│  ├─ 参数表读取与写入
│  └─ ParamScripts 外置执行
└─ Overlay (src/grace_test_overlay.cpp)
   └─ DX12 Hook 与伤害弹字渲染链路
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
