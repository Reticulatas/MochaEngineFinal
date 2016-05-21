using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace MochaEditor
{
     [ValueConversion(typeof(MochaInterface.Color), typeof(System.Windows.Media.Color))]
    public class ColorConverter : IValueConverter
    {
         public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
         {
             if (!(value is MochaInterface.Color))
                 return null;
             MochaInterface.Color c = value as MochaInterface.Color;
             return System.Windows.Media.Color.FromScRgb(c.a,c.r,c.g,c.b);
         }

         public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
         {
             if (!(value is System.Windows.Media.Color))
                 return null;
             System.Windows.Media.Color c = (System.Windows.Media.Color)value;
             return new MochaInterface.Color((float)c.R / 255.0f, (float)c.G / 255.0f, (float)c.B / 255.0f, (float)c.A / 255.0f);
         }
    }
}
