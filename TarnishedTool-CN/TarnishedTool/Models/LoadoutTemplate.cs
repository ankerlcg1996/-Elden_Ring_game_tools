// 

using System.Collections.Generic;

namespace TarnishedTool.Models;

public class LoadoutTemplate
{
    public string Name { get; set; }
    public List<ItemTemplate> Items { get; set; } = new();
}