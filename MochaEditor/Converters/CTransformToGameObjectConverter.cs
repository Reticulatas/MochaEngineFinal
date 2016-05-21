using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace MochaEditor.Converters
{
    public class CTransformToGameObjectConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            return GameObject.From((value as MochaInterface.CTransform).gameObject);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            return (value as GameObject).transform;
        }
    }

    public class CTransformListToGameObjectListConverter : IValueConverter
    {
        static CTransformToGameObjectConverter individual_conv = new CTransformToGameObjectConverter();
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            ObservableCollection<GameObject> objs = new ObservableCollection<GameObject>();
            var trans_list = value as MochaInterface.CTransformVector;
            foreach (var ct in trans_list)
            {
                objs.Add(GameObject.From(ct.gameObject));
            }
            return objs;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            throw new NotImplementedException();
        }
    }
}
