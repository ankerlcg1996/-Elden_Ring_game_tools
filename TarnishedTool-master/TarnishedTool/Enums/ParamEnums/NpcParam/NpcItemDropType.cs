// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.NpcParam;

public enum NpcItemDropType : ushort
{
    [Description("Corpse Emission")]
    CorpseEmission = 0,
    [Description("Item Display")]
    ItemDisplay = 1
}