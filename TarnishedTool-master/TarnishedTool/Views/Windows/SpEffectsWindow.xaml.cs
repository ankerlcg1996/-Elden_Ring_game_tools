// 

using System.Windows;
using TarnishedTool.Utilities;

namespace TarnishedTool.Views.Windows;

public partial class SpEffectsWindow : TopmostWindow
{
    public SpEffectsWindow()
    {
        InitializeComponent();
        
        if (Application.Current.MainWindow != null)
        {
            Application.Current.MainWindow.Closing += (sender, args) => { Close(); };
        }

        Loaded += (s, e) =>
        {
            if (SettingsManager.Default.TargetSpEffectWindowLeft > 0)
                Left = SettingsManager.Default.TargetSpEffectWindowLeft;
        
            if (SettingsManager.Default.TargetSpEffectWindowTop > 0)
                Top = SettingsManager.Default.TargetSpEffectWindowTop;
            
            AlwaysOnTopCheckBox.IsChecked = SettingsManager.Default.TargetSpEffectAlwaysOnTop;
        };
    }
    
    protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
    {
        base.OnClosing(e);


        SettingsManager.Default.TargetSpEffectWindowLeft = Left;
        SettingsManager.Default.TargetSpEffectWindowTop = Top;
        SettingsManager.Default.TargetSpEffectAlwaysOnTop = AlwaysOnTopCheckBox.IsChecked ?? false;
        SettingsManager.Default.Save();
    }
}