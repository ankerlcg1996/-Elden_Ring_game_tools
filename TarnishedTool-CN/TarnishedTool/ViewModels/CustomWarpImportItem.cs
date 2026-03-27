// 

using System.Collections.Generic;
using TarnishedTool.Models;

namespace TarnishedTool.ViewModels;

public class CustomWarpImportItem(string category, List<BlockWarp> warps, bool alreadyExists) : BaseViewModel
{
    public string Category { get; } = category;
    public List<BlockWarp> Warps { get; } = warps;
    public int WarpCount => Warps?.Count ?? 0;
    public bool AlreadyExists { get; } = alreadyExists;

    private bool _isSelected = !alreadyExists;

    public bool IsSelected
    {
        get => _isSelected;
        set => SetProperty(ref _isSelected, value);
    }
}