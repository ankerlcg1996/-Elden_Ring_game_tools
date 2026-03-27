using System;
using System.Globalization;
using System.Windows.Data;

namespace TarnishedTool.Converters
{
    public class BoolToShowHideConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool isShowing)
            {
                return isShowing ? "隐藏原版数值" : "显示原版数值";
            }
            return "显示原版数值";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return Binding.DoNothing;
        }
    }
}
