using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.CharaInitParam;

public enum NpcDrawType : sbyte
{
    Normal = 0,
    [Description("White Phantom")]
    WhitePhantom = 1,
    [Description("Black Phantom")]
    BlackPhantom = 2,
    Intruder = 3
}