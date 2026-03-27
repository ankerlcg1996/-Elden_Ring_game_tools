// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.SpEffect;

public enum SpEffectWepChangeParam : byte
{
    None = 0,
    [Description("Right-hand")]
    RightHand = 1,
    [Description("Left-hand")]
    LeftHand = 2,
    Self = 3,
    Kick = 4
}