// 

namespace TarnishedTool.Models;

public class Weapon : Item
{
    public ushort WeaponType { get; set; }
    public byte GemMountType { get; set; }
    public byte UpgradeType { get; set; }
    
    public bool CanApplyAow => GemMountType == 1 || GemMountType == 2;
}