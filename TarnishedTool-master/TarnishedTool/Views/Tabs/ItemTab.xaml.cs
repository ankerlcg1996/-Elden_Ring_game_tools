// 

using System.Linq;
using System.Windows.Controls;
using System.Windows.Input;
using TarnishedTool.Models;
using TarnishedTool.ViewModels;

namespace TarnishedTool.Views.Tabs;

public partial class ItemTab
{
    public ItemTab(ItemViewModel itemViewModel)
    {
        InitializeComponent();
        DataContext = itemViewModel;
    }

    private void Item_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
        if (DataContext is ItemViewModel vm && vm.SpawnItemCommand.CanExecute(null))
        {
            vm.SpawnItemCommand.Execute(null);
        }
    }

    private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (DataContext is ItemViewModel vm && sender is ListView lv)
        {
            vm.ItemSelection.SelectedItems = lv.SelectedItems.Cast<Item>().ToList();
        }
    }
}