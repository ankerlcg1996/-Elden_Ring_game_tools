// 

namespace TarnishedTool.ViewModels;

public class LuaNumberViewModel : BaseViewModel
{
    public int Index { get; set; }
    
    private float _value;
    public float Value
    {
        get => _value;
        set => SetProperty(ref _value, value);
    }
}