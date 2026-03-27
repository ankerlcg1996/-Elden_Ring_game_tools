// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.SpEffect;

public enum SpEffectWetConditionDepth : byte
{
    None = 0,
    [Description("Full Body")]
    FullBody = 1,
    [Description("Lower Body")]
    LowerBody = 2
}