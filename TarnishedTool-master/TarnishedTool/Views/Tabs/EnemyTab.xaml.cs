using TarnishedTool.ViewModels;

namespace TarnishedTool.Views.Tabs;

public partial class EnemyTab
{
    public EnemyTab(EnemyViewModel enemyViewModel)
    {
        InitializeComponent();
        DataContext = enemyViewModel;
    }
}