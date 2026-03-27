// 

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Input;
using TarnishedTool.Core;
using TarnishedTool.Models;

namespace TarnishedTool.ViewModels;

public class AttackInfoViewModel : BaseViewModel
{
    private const int MaxVisibleEntries = 500;
    private readonly List<AttackInfoEntry> _fullHistory = new();
    public ObservableCollection<AttackInfoEntry> VisibleLog { get; } = new();
    
    public AttackInfoViewModel()
    {
        ClearCommand = new DelegateCommand(Clear);
        ExportCommand = new DelegateCommand(Export);
    }

    #region Commands
    
    public ICommand ClearCommand { get; }
    public ICommand ExportCommand { get; }

    #endregion
    

    #region Public Methods
    
    public void AddAttacks(List<AttackInfo> attacks)
    {
        foreach (var attack in attacks)
        {
            AddAttack(attack);
        }
    }

    

    #endregion

    #region Private Methods
    
    private void AddAttack(AttackInfo attack)
    {
        var entry = new AttackInfoEntry(attack);
        _fullHistory.Add(entry);

        Application.Current.Dispatcher.Invoke(() =>
        {
            VisibleLog.Insert(0, entry);

            if (VisibleLog.Count > MaxVisibleEntries)
                VisibleLog.RemoveAt(VisibleLog.Count - 1);
        });
    }
    
    private void Clear()
    {
        _fullHistory.Clear();
        VisibleLog.Clear();
    }
    
    private void Export()
    {
        var dialog = new Microsoft.Win32.SaveFileDialog
        {
            FileName = $"AttackLog_{DateTime.Now:yyyyMMdd_HHmmss}",
            DefaultExt = ".csv",
            Filter = "CSV files (*.csv)|*.csv"
        };

        if (dialog.ShowDialog() != true) return;

        var sb = new StringBuilder();
        sb.AppendLine("Id,EnemyId,AttackType,TotalDamage,Physical,Fire,Magic,Lightning,Holy,RawPhysical,RawFire,RawMagic,RawLightning,RawHoly,PoiseDamage,RawSplit,FinalSplit");

        foreach (var e in _fullHistory)
        {
            sb.AppendLine($"{e.Id},{e.EnemyId},{e.AttackType},{e.TotalDamage},{e.PhysicalDamage},{e.FireDamage},{e.MagicDamage},{e.LightningDamage},{e.HolyDamage},{e.RawPhysicalDamage:F2},{e.RawFireDamage:F2},{e.RawMagicDamage:F2},{e.RawLightningDamage:F2},{e.RawHolyDamage:F2},{e.PoiseDamage:F2},{e.RawSplitText},{e.FinalSplitText}");
        }

        File.WriteAllText(dialog.FileName, sb.ToString());
    }

    #endregion
}