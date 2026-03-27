// 

using static TarnishedTool.GameIds.Emevd;

namespace TarnishedTool.Interfaces;

public interface IEmevdService
{
    void ExecuteEmevdCommand(EmevdCommand command);
}