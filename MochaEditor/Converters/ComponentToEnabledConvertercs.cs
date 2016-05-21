using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace MochaEditor
{
    public partial class ComponentToEnabledConvertercs : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return false;
            var comp = value as MochaInterface.Component;

            if (comp == null)
                throw new InvalidOperationException("Cannot convert non-component to enabled");

            bool enabled = comp.isEnabled();
            return enabled;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
