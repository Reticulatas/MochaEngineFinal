using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Reflection;
using System.Collections.ObjectModel;

namespace MochaEditor
{
    /// <summary>
    /// Not working - returns single element n times
    /// </summary>
    public class SetToColConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            ObservableCollection<object> objs = new ObservableCollection<object>();
            var iter = value.GetType().GetMethod("create_iterator_begin").Invoke(value, null);
            uint size = (uint)value.GetType().GetMethod("size").Invoke(value, null);
            if (size == 0)
                return objs;
            MethodInfo nextM = value.GetType().GetMethod("get_next_key");
            object[] parms = {iter};
            object obj = null;
            for (int i = 0; i < size; ++i )
            {
                obj = nextM.Invoke(value, parms);
                objs.Add(obj);
            }
            value.GetType().GetMethod("destroy_iterator").Invoke(value, parms);
            return objs;            
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
