// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.SpEffect;

public enum SpEffectChangeTeamType : sbyte
{
    None = 0,
    Live = 1,
    [Description("White Ghost")]
    WhiteGhost = 2,
    [Description("Black Ghost")]
    BlackGhost = 3,
    [Description("Grey Ghost")]
    GreyGhost = 4,
    [Description("Wandering Ghost")]
    WanderingGhost = 5,
    Enemy = 6,
    Boss = 7,
    Ally = 8,
    [Description("Hostile Ally")]
    HostileAlly = 9,
    Decoy = 10,
    [Description("Decoy-like")]
    DecoyLike = 11,
    [Description("Battle Ally")]
    BattleAlly = 12,
    Invader13 = 13,
    Neutral = 14,
    Charmed = 15,
    Invader16 = 16,
    Invader17 = 17,
    Invader18 = 18,
    Host = 19,
    [Description("Co-op")]
    Coop = 20,
    Hostile = 21,
    [Description("Wandering Phantom")]
    WanderingPhantom = 22,
    [Description("Enemy 1")]
    Enemy1 = 23,
    [Description("Enemy 2")]
    Enemy2 = 24,
    [Description("Strong Enemy")]
    StrongEnemy = 25,
    [Description("Friendly NPC")]
    FriendlyNpc = 26,
    [Description("Hostile NPC")]
    HostileNpc = 27,
    [Description("Co-op NPC")]
    CoopNpc = 28,
    Indiscriminate = 29,
    Object = 30,
    [Description("Co-op Mad Phantom")]
    CoopMadPhantom = 31,
    [Description("Invader Mad Phantom")]
    InvaderMadPhantom = 32,
    [Description("Arch Enemy Team")]
    ArchEnemyTeam = 33,
    [Description("Spirit Summon")]
    SpiritSummon = 47,
    Unknown48 = 48,
    Unknown51 = 51,
    Unknown54 = 54,
    Unknown55 = 55,
    Unknown59 = 59,
    Unknown60 = 60,
    Unknown63 = 63,
    Unknown66 = 66
}