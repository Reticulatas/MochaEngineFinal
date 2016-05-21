using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace MochaEditor
{
    public class PropertyNameSanitizerConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            if (!(value is string))
                return null;
            string name = value as string;

            if (name.Count() <= 2)
                return name;
            //rip out the m
            if (name[0] == 'm')
                name = name.Substring(1);

            //split at caps
            for (int i = 1; i < name.Count(); ++i)
                if (char.IsUpper(name[i]))
                    name = name.Insert(i++, " ");

            //convert first letter to uppercase
            if (char.IsLower(name[0]))
                name = char.ToUpper(name[0]) + name.Substring(1);

            name.Trim();

            return name;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
