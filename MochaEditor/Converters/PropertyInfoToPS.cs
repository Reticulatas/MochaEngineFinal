using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace MochaEditor
{
     [ValueConversion(typeof(PropertyValueConverter.PropertyInfo), typeof(object))]
    public class PropertyInfoToPS : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            PropertyValueConverter.PropertyInfo pi = value as PropertyValueConverter.PropertyInfo;

            if (parameter == null)
            {
                return pi.PropertyValue;
            }
            else
            {
                PropertyInfo p = pi.PropertyType.GetProperty(parameter as string);
                return p.GetValue(pi.PropertyValue);
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            PropertyValueConverter.PropertyInfo pi = value as PropertyValueConverter.PropertyInfo;
            if (parameter == null)
                return System.Convert.ChangeType(value, pi.PropertyType);
            //find the changed value and put it back?
            PropertyInfo p = pi.PropertyType.GetProperty(parameter as string);
            p.SetValue(pi.PropertyValue, System.Convert.ChangeType(value, p.PropertyType));
            return pi.PropertyValue;
        }
    }

     [ValueConversion(typeof(float), typeof(double))]
     public class FloatToDoubleConverter : IValueConverter
     {
         public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
         {
             if (value == null)
                 return null;
             return System.Convert.ChangeType(value, typeof(double));
         }

         public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
         {
             if (value == null)
                 return null;
             return System.Convert.ChangeType(value, typeof(float));
         }
     }
}
