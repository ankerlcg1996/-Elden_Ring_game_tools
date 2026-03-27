// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.SpEffect;

public enum SpEffectSpCategory : ushort
{
    None = 0,
    [Description("Persist through Death")]
    PersistThroughDeath = 1,
    [Description("Stack Self")]
    StackSelf = 10,
    [Description("Reset on Apply")]
    ResetOnApply = 20,
    [Description("Remove Previous")]
    RemovePrevious100 = 100,
    [Description("Remove Previous")]
    RemovePrevious110 = 110,
    [Description("Remove Previous")]
    RemovePrevious120 = 120,
    [Description("Remove Previous")]
    RemovePrevious130 = 130,
    [Description("Remove Previous")]
    RemovePrevious131 = 131,
    [Description("Remove Previous")]
    RemovePrevious132 = 132,
    [Description("Remove Previous")]
    RemovePrevious133 = 133,
    [Description("Remove Previous")]
    RemovePrevious140 = 140,
    [Description("Remove Previous")]
    RemovePrevious150 = 150,
    [Description("Remove Previous")]
    RemovePrevious151 = 151,
    [Description("Remove Previous")]
    RemovePrevious152 = 152,
    [Description("Remove Previous")]
    RemovePrevious153 = 153,
    [Description("Remove Previous")]
    RemovePrevious154 = 154,
    [Description("Remove Previous")]
    RemovePrevious155 = 155,
    [Description("Remove Previous")]
    RemovePrevious156 = 156,
    [Description("Remove Previous")]
    RemovePrevious157 = 157,
    [Description("Remove Previous")]
    RemovePrevious158 = 158,
    [Description("Remove Previous")]
    RemovePrevious159 = 159,
    [Description("Remove Previous")]
    RemovePrevious160 = 160,
    [Description("Remove Previous")]
    RemovePrevious161 = 161,
    [Description("Remove Previous")]
    RemovePrevious162 = 162,
    [Description("Remove Previous")]
    RemovePrevious163 = 163,
    [Description("Remove Previous")]
    RemovePrevious164 = 164,
    [Description("Remove Previous")]
    RemovePrevious165 = 165,
    [Description("Remove Previous")]
    RemovePrevious166 = 166,
    [Description("Remove Previous")]
    RemovePrevious167 = 167,
    [Description("Remove Previous")]
    RemovePrevious168 = 168,
    [Description("Remove Previous")]
    RemovePrevious174 = 174,
    [Description("Remove Previous w/ Matching Priority")]
    RemovePreviousMatchingPriority = 200,
    [Description("Remove Previous")]
    RemovePrevious201 = 201,
    [Description("Apply Highest (Category Priority)")]
    ApplyHighest1000 = 1000,
    [Description("Apply Highest (Category Priority)")]
    ApplyHighest1001 = 1001,
    [Description("Apply Highest (Category Priority)")]
    ApplyHighest1002 = 1002,
    [Description("Apply Highest (Category Priority)")]
    ApplyHighest1003 = 1003,
    [Description("Apply Highest (Category Priority)")]
    ApplyHighest1004 = 1004,
    [Description("Apply Highest (Category Priority)")]
    ApplyHighest1005 = 1005,
    [Description("Apply Highest (Category Priority)")]
    ApplyHighest1006 = 1006,
    [Description("Apply First")]
    ApplyFirst10000 = 10000,
    [Description("Apply First (Damage Level Change)")]
    ApplyFirstDamageLevelChange = 10001,
    [Description("Apply First")]
    ApplyFirst10002 = 10002,
    [Description("Apply First (Blood Loss)")]
    ApplyFirstBloodLoss = 10003,
    [Description("Apply First (Poison)")]
    ApplyFirstPoison = 10004,
    [Description("Apply First (Scarlet Rot)")]
    ApplyFirstScarletRot = 10005,
    [Description("Apply First")]
    ApplyFirst10006 = 10006,
    [Description("Apply First (Frostbite)")]
    ApplyFirstFrostbite = 10007,
    [Description("Apply First (Death Blight)")]
    ApplyFirstDeathBlight = 10008,
    [Description("Apply First (Sleep)")]
    ApplyFirstSleep = 10009,
    [Description("Apply First (Madness)")]
    ApplyFirstMadness = 10010
}
