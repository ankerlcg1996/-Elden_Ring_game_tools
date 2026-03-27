// 

using TarnishedTool.GameIds;

namespace TarnishedTool.Models;

public class ShopCommand(bool isDlc, string name, EzState.TalkCommand command)
{
    public bool IsDlc { get; set; } = isDlc;
    public string Name { get; set; } = name;
    public EzState.TalkCommand Command { get; set; } = command;
}