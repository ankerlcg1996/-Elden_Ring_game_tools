// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.NpcParam;

public enum NpcWheelRotType : byte
{
    [Description("Type 0 - Default")]
    Type0Default = 0,
    [Description("Type 1 - Flamethrower Tank")]
    Type1FlamethrowerTank = 1,
    [Description("Type 2 - Abductor Virgin")]
    Type2AbductorVirgin = 2,
    [Description("Type 3 - Silver Tear Orb")]
    Type3SilverTearOrb = 3
}