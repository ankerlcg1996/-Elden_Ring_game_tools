// 

using System.Numerics;

namespace TarnishedTool.Models;

public class PosWithHurtbox(Vector3 position, float capsuleRadius)
{
    public Vector3 position = position;
    public  float capsuleRadius = capsuleRadius;
}