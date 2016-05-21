using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;


namespace MochaEditor
{
    [ValueConversion(typeof(object), typeof(object))]
    public class MochaHintConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            PropertyValueConverter.PropertyInfo pi = value as PropertyValueConverter.PropertyInfo;
            if (pi == null)
                throw new InvalidOperationException("Value must be property info struct");

            object[] attribs = pi.PropertyType.GetCustomAttributes(true);
            foreach (object attribute in attribs)
            {
                if (attribute is MochaHint)
                {
                    MochaHint hint = attribute as MochaHint;
                    pi.PropertyValue = (hint.converter as IValueConverter).Convert(pi.PropertyValue, targetType, parameter, null);
                    return pi;
                }
            }
            return pi;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            foreach (object attribute in targetType.GetCustomAttributes(false))
            {
                if (attribute is MochaHint)
                {
                    MochaHint hint = attribute as MochaHint;
                    return (hint.converter as IValueConverter).ConvertBack(value, targetType, parameter, null);
                }
            }
            return value;
        }
    }
}
