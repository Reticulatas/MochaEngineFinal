using System.Windows.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Xna.Framework;
using MochaInterface;
using MochaEditor;
using MochaCore;
using System.Collections.ObjectModel;

namespace MochaEditor
{
    class Vector3ToMochaVec3Conv : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            var vi = (PropertyValueConverter.PropertyInfo)value;
            var v = (Vector3)vi.PropertyValue;
            ObservableCollection<float> oc = new ObservableCollection<float>();

            oc.Add(v.X);
            oc.Add(v.Y);
            oc.Add(v.Z);

            return oc;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            var mv = value as ObservableCollection<float>;
            Vector3 v = new Vector3(mv[0], mv[1], mv[2]);
            return v;
        }
    }
}
