// 

using System.Collections.Generic;
using TarnishedTool.Models;

namespace TarnishedTool.Interfaces;

public interface IAttackInfoService
{
    void ToggleAttackInfoHook(bool isEnabled);
    List<AttackInfo> PollAttackInfo();
}