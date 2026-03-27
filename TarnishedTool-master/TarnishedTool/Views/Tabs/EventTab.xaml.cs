using TarnishedTool.ViewModels;

namespace TarnishedTool.Views.Tabs
{
    public partial class EventTab
    {
        
        public EventTab(EventViewModel eventViewModel)
        {
            InitializeComponent();
            DataContext = eventViewModel;
        }
    }
}