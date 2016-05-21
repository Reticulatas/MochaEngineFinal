using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace MochaEditor
{
    [ValueConversion(typeof(float), typeof(float))]
    public class RadToDegConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            if (value is float || value is double)
            {
                var val = (float)value;
                return (val * Math.PI) / 180.0f;
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            if (value is float || value is double)
            {
                var val = (float)value;
                return (val * 180.0f) / Math.PI;
            }
            return null;
        }
    }
}
