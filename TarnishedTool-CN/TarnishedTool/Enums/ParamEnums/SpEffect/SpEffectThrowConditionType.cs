// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.SpEffect;

public enum SpEffectThrowConditionType : byte
{
    None = 0,
    Forbidden = 1,
    Parry = 2,
    Unknown3 = 3,
    Fall = 4,
    [Description("Hornet Ring")]
    HornetRing = 5,
    Lifedrain = 6,
    Unknown7 = 7,
    Unknown8 = 8,
    Unknown9 = 9,
    Backstab = 10,
    Unknown11 = 11,
    Unknown12 = 12,
    Unknown13 = 13,
    Unknown14 = 14,
    Unknown15 = 15
}