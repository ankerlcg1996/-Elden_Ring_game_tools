// 

namespace TarnishedTool.Interfaces;

public interface IItemService
{
    void SpawnItem(int itemId, int quantity, int aowId, bool isQuantityAdjustable, int maxQuantity);
}