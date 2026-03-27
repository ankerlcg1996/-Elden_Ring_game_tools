using TarnishedTool.Utilities;
using TarnishedTool.ViewModels;

namespace TarnishedTool.Views.Tabs
{
    public partial class TargetTab
    {
        private readonly TargetViewModel _targetViewModel;

        public TargetTab(TargetViewModel targetViewModel)
        {
            InitializeComponent();
            _targetViewModel = targetViewModel;
            DataContext = _targetViewModel;
            
            InitializeUpDownHelpers();
        }
        
        private void InitializeUpDownHelpers()
        {
            _ = new UpDownHelper<double>(
                SpeedUpDown,
                _targetViewModel.SetSpeed
            );
        }
    }
}