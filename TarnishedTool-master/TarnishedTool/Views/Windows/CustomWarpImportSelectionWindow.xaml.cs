using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using TarnishedTool.Models;
using TarnishedTool.ViewModels;

namespace TarnishedTool.Views.Windows;

public partial class CustomWarpImportSelectionWindow : TopmostWindow
{
    public CustomWarpImportSelectionViewModel ViewModel { get; }

    public CustomWarpImportSelectionWindow(
        Dictionary<string, List<BlockWarp>> importedWarps,
        Dictionary<string, List<BlockWarp>> existingWarps)
    {
        InitializeComponent();

        ViewModel = new CustomWarpImportSelectionViewModel(importedWarps, existingWarps);
        DataContext = ViewModel;

        Loaded += (_, _) =>
        {
            foreach (var item in ViewModel.Categories)
            {
                if (item.IsSelected)
                {
                    CategoryListView.SelectedItems.Add(item);
                }
            }
        };
    }

    private void CategoryItem_Selected(object sender, RoutedEventArgs e)
    {
        if (sender is ListViewItem { DataContext: CustomWarpImportItem item })
        {
            item.IsSelected = true;
        }
    }

    private void CategoryItem_Unselected(object sender, RoutedEventArgs e)
    {
        if (sender is ListViewItem { DataContext: CustomWarpImportItem item })
        {
            item.IsSelected = false;
        }
    }

    private void Cancel_Click(object sender, RoutedEventArgs e)
    {
        DialogResult = false;
        Close();
    }

    private void Import_Click(object sender, RoutedEventArgs e)
    {
        DialogResult = true;
        Close();
    }
}