// 

using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using TarnishedTool.Models;

namespace TarnishedTool.ViewModels;

public class CustomWarpImportSelectionViewModel : BaseViewModel
{
    public ObservableCollection<CustomWarpImportItem> Categories { get; }

    public CustomWarpImportSelectionViewModel(
        Dictionary<string, List<BlockWarp>> importedWarps,
        Dictionary<string, List<BlockWarp>> existingWarps)
    {
        Categories = new ObservableCollection<CustomWarpImportItem>(
            importedWarps.Select(kvp => new CustomWarpImportItem(
                kvp.Key,
                kvp.Value,
                existingWarps.ContainsKey(kvp.Key)))
        );

        foreach (var category in Categories)
        {
            category.PropertyChanged += OnCategorySelectionChanged;
        }

        UpdateSelectedCount();
    }

    #region Properties

    private ConflictResolution _selectedConflictResolution = ConflictResolution.Skip;

    public ConflictResolution SelectedConflictResolution
    {
        get => _selectedConflictResolution;
        set => SetProperty(ref _selectedConflictResolution, value);
    }

    private int _selectedCount;

    public int SelectedCount
    {
        get => _selectedCount;
        private set
        {
            SetProperty(ref _selectedCount, value);
            OnPropertyChanged(nameof(CanImport));
        }
    }

    public bool CanImport => SelectedCount > 0;

    public bool HasConflicts => Categories.Any(c => c.IsSelected && c.AlreadyExists);

    #endregion

    #region Private Methods

    private void OnCategorySelectionChanged(object sender, PropertyChangedEventArgs e)
    {
        if (e.PropertyName == nameof(CustomWarpImportItem.IsSelected))
        {
            UpdateSelectedCount();
            OnPropertyChanged(nameof(HasConflicts));
        }
    }

    private void UpdateSelectedCount()
    {
        SelectedCount = Categories.Count(c => c.IsSelected);
    }

    #endregion

    #region Public Methods

    public Dictionary<string, List<BlockWarp>> GetSelectedCategories()
    {
        return Categories
            .Where(c => c.IsSelected)
            .ToDictionary(c => c.Category, c => c.Warps);
    }

    #endregion
}