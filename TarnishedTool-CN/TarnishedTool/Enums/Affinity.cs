// 

using System;
using System.Collections.Generic;

namespace TarnishedTool.Enums;

[Flags]
public enum Affinity : ushort
{
    Standard  = 1 << 0,
    Heavy     = 1 << 1,
    Keen      = 1 << 2,
    Quality   = 1 << 3,
    Fire      = 1 << 4,
    FlameArt  = 1 << 5,
    Lightning = 1 << 6,
    Sacred    = 1 << 7,
    Magic     = 1 << 8,
    Cold      = 1 << 9,
    Poison    = 1 << 10,
    Blood     = 1 << 11,
    Occult    = 1 << 12,
}

public static class AffinityExtensions
{
    private static readonly Dictionary<Affinity, int> IdOffsets = new()
    {
        [Affinity.Standard]  = 0,
        [Affinity.Heavy]     = 100,
        [Affinity.Keen]      = 200,
        [Affinity.Quality]   = 300,
        [Affinity.Fire]      = 400,
        [Affinity.FlameArt]  = 500,
        [Affinity.Lightning] = 600,
        [Affinity.Sacred]    = 700,
        [Affinity.Magic]     = 800,
        [Affinity.Cold]      = 900,
        [Affinity.Poison]    = 1000,
        [Affinity.Blood]     = 1100,
        [Affinity.Occult]    = 1200,
    };

    public static int GetIdOffset(this Affinity affinity) => IdOffsets[affinity];
}