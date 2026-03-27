// 

using System;
using System.Collections.Generic;
using System.Linq;
using TarnishedTool.Enums;

namespace TarnishedTool.Models;

public class AshOfWar
{
    public int Id { get; set; }
    public string Name { get; set; }
    public Affinity AvailableAffinities { get; set; }
    public byte[] WeaponTypeFlags { get; set; }
    public static AshOfWar None { get; } = new() { Id = -1, Name = "None" };
    
    private static readonly Dictionary<ushort, int> WeaponTypeToBit = new()
    {
        [1] = 0,    [3] = 1,    [5] = 2,    [7] = 3,
        [9] = 4,    [0xB] = 5,  [0xD] = 6,  [0xE] = 7,
        [0xF] = 8,  [0x10] = 9, [0x11] = 10,[0x13] = 11,
        [0x15] = 12,[0x17] = 13,[0x18] = 14,[0x19] = 15,
        [0x1B] = 16,[0x1C] = 17,[0x1D] = 18,[0x1F] = 19,
        [0x23] = 20,[0x25] = 21,[0x27] = 22,[0x29] = 23,
        [0x32] = 24,[0x33] = 25,[0x35] = 26,[0x37] = 27,
        [0x38] = 28,[0x39] = 29,[0x3B] = 30,[0x3D] = 31,
        [0x41] = 32,[0x43] = 33,[0x45] = 34,[0x57] = 35,
        [0x58] = 36,[0x59] = 37,[0x5A] = 38,[0x5B] = 39,
        [0x5C] = 40,[0x5D] = 41,[0x5E] = 42,[0x5F] = 43,
    };
    
    public bool SupportsWeaponType(ushort weaponType)
    {
        if (!WeaponTypeToBit.TryGetValue(weaponType, out int bitIndex))
            return false;
        
        return (WeaponTypeFlags[bitIndex / 8] & (1 << (bitIndex % 8))) != 0;
    }
    
    public bool SupportsAffinity(Affinity affinity)
    {
        return (AvailableAffinities & affinity) != 0;
    }
    
    public IEnumerable<Affinity> GetAvailableAffinities()
    {
        return Enum.GetValues(typeof(Affinity)).Cast<Affinity>().Where(SupportsAffinity);
    }
}