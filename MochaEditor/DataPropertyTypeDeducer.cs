using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Collections.ObjectModel;
using System.Windows.Media;

namespace MochaEditor
{
    /* Deduces rough type for Data Node display */
    [ValueConversion(typeof(object), typeof(string))]
    public class DataPropertyTypeDeducer : IValueConverter
    {
        public static readonly IValueConverter Instance = new DataPropertyTypeDeducer();

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            if (value is int || value is uint || value is UInt32 || value is char || value is Int32 || value is long)
                return "WholeNumber";
            if (value is float || value is double)
                return "Decimal";
            if (value is string || value is String)
                return "String";
            if (value is bool)
                return "Boolean";
            if (value is ScriptVector)
                return "ScriptVector";
            if (value.GetType().IsEnum == true)
                return "Enum";
            if (typeof(System.Collections.IEnumerable).IsAssignableFrom(value.GetType()))
            {
                string itemType = "";
                foreach (object item in (value as System.Collections.IEnumerable)) {
                    itemType = item.GetType().Name;
                    break;
                }
                return "Container|"+itemType;
            }

            return value.GetType().Name;
            //return "Unknown";
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new Exception("The method or operation is not possible.");
        }
    }
}
