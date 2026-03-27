// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.NpcParam;

public enum NpcMoveType : byte
{
    Immobile = 0,
    [Description("All Directions")]
    AllDirections = 1,
    [Description("Forward and Backwards Only")]
    ForwardAndBackwardsOnly = 2,
    Biped = 3,
    Quadruped = 4,
    Flight = 5,
    Hover = 6,
    [Description("Hover + Biped")]
    HoverBiped = 7,
    [Description("Flight + Biped")]
    FlightBiped = 8,
    [Description("Boss Biped")]
    BossBiped = 9,
    Attach = 10
}