// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.NpcThinkParam;

public enum NpcThinkGoalAction : byte
{
    [Description("Wait for Battle Start Distance")]
    WaitForBattleStartDistance = 0,
    [Description("Look at Source")]
    LookAtSource = 1,
    [Description("Approach Source")]
    ApproachSource = 2,
    [Description("Run to Source")]
    RunToSource = 3,
    [Description("Immediate Battle AI")]
    ImmediateBattleAi = 4
}