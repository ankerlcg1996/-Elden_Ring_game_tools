// 

using System.Numerics;

namespace TarnishedTool.Models;

public class Position(uint blockId, Vector3 coords, float angle)
{
    public uint BlockId { get; set; } = blockId;
    public Vector3 Coords { get; set; } = coords;
    public float Angle { get; set; } = angle;
}