using TarnishedTool.ViewModels;

namespace TarnishedTool.Views.Tabs
{
    public partial class UtilityTab
    {
        public UtilityTab(UtilityViewModel utilityViewModel)
        {
            InitializeComponent();
            DataContext = utilityViewModel;
        }
    }
}