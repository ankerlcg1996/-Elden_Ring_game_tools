// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.NpcParam;

public enum NpcHitStopType : byte
{
    [Description("No Hit Stop")]
    NoHitStop = 0,
    [Description("Partial Hit Stop")]
    PartialHitStop = 1,
    [Description("Hit Stop")]
    HitStop = 2
}