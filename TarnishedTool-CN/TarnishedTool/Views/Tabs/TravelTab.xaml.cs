using System.Windows.Controls;
using System.Windows.Input;
using TarnishedTool.ViewModels;

namespace TarnishedTool.Views.Tabs
{
    public partial class TravelTab : UserControl
    {
        public TravelTab(TravelViewModel travelViewModel)
        {
            InitializeComponent();
            DataContext = travelViewModel;
        }

        private void GraceItem_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (DataContext is TravelViewModel vm && vm.GraceWarpCommand.CanExecute(null))
            {
                vm.GraceWarpCommand.Execute(null);
            }
        }

        private void BossItem_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (DataContext is TravelViewModel vm && vm.BossWarpCommand.CanExecute(null))
            {
                vm.BossWarpCommand.Execute(null);
            }
        }

        private void CustomItem_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (DataContext is TravelViewModel vm && vm.CustomWarpCommand.CanExecute(null))
            {
                vm.CustomWarpCommand.Execute(null);
            }
        }
    }
}