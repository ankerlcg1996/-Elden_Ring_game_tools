# TarnishedTool-master AOB 特征提取说明

- 支持游戏版本数量: 25
- Pattern.cs 提取到的 AOB Pattern 数量: 84
- 说明: 仓库中 AOB 主要集中在 `Memory/Pattern.cs`，并未按每个版本分别维护整套 AOB；版本差异主要由 `Offsets.cs` 处理。

## 支持的游戏版本
- Version1_2_0, //1.02
- Version1_2_1, // 1.2.1
- Version1_2_2, // 1.2.2
- Version1_2_3, //1.2.3
- Version1_3_0, //1.3.0
- Version1_3_1, // 1.3.1
- Version1_3_2, // 1.3.2 + 1.3.3
- Version1_4_0, // 1.4.0
- Version1_4_1, // 1.4.1
- Version1_5_0, // 1.5.0
- Version1_6_0, // 1.6.0
- Version1_7_0, // 1.7.1 + 1.7.0
- Version1_8_0, // 1.8.0
- Version1_8_1, // 1.8.1
- Version1_9_0, //1.09
- Version1_9_1, //1.09.1
- Version2_0_0, //1.10
- Version2_0_1, //1.10.1
- Version2_2_0, // 1.12.2 + 1.12.1
- Version2_2_3, // 1.12.4
- Version2_3_0, // 1.13.1 + 1.13.2
- Version2_4_0, // 1.14.1 
- Version2_5_0, //  1.15
- Version2_6_0, // 1.16
- Version2_6_1, // 1.16.1

## 与补丁阶段相关的 AOB Pattern
- LoadScreenMsgLookup | mask=xxxxxxxx | bytes=8
- LoadScreenMsgLookupEarlyPatches | mask=xxxxxxxx????xxxxxxxx | bytes=20
- LoadScreenMsgLookupMidPatches | mask=xxxxxxxx | bytes=8

## 导出文件
- `AOB特征_原始提取.csv`: 全量 Pattern 列表（name/mask/bytes/addressing mode/offset 参数）
- `支持的游戏版本_来自GameVersion.txt`: 版本枚举列表
