using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Collections.ObjectModel;

namespace MochaEditor
{
    public sealed class EnumToObservableConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            ObservableCollection<string> enumValues = new ObservableCollection<string>();
            if (value == null)
                return enumValues;

            foreach (string s in value.GetType().GetEnumNames())
                enumValues.Add(s);

            return enumValues;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }

    public sealed class EnumValueToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value.GetType().GetEnumName(value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            int i = 0;
            foreach (string s in (parameter as Type).GetEnumNames())
            {
                if (s == value as string)
                    break;
                ++i;
            }

            return (parameter as Type).GetEnumValues().GetValue(i);
        }
    }
}
