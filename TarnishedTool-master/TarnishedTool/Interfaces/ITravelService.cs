// 

using TarnishedTool.Models;

namespace TarnishedTool.Interfaces;

public interface ITravelService
{
    void Warp(Grace grace);
    void WarpToBlockId(Position position);
    void ToggleShowAllGraces(bool isEnabled);
    void ToggleShowAllMaps(bool isEnabled);
    void ToggleNoMapAcquiredPopups(bool isEnabled);
}