using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for MultiPropertiesControl.xaml
    /// </summary>
    public class MultiPropertyObject
    {
        object _object;
        public object merged_object
        {
            get
            {
                return _object;
            }
            set
            {
                _object = value;
            }
        }
    }

    public class Multi_MultiComponentPackToMultiPropertyPackListConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            var compack = value as MultiComponentPack;

            List<MultiPropertyPack> pack = new List<MultiPropertyPack>();

            if (compack.merged_components.Count == 0)
                return null;

            MochaInterface.Component c = compack.merged_components[0];

            PropertyValueConverter pvc = new PropertyValueConverter();
            var pi_ret = new ObservableCollection<PropertyValueConverter.PropertyInfo>();
            pi_ret = pvc.Convert(c, null, "Component", null) as ObservableCollection<PropertyValueConverter.PropertyInfo>; 

            foreach (var pi in pi_ret)
            {
                MultiPropertyPack ppack = new MultiPropertyPack();
                ppack.merged_data = pi.PropertyValue;
                MultiPropertyInfoWrapper merged_pi = new MultiPropertyInfoWrapper();
                merged_pi.merged_name = pi.PropertyName;
                merged_pi.merged_value = pi.PropertyValue;
                merged_pi.participating_objs = new List<object>();
                foreach (var comp in compack.merged_components)
                    merged_pi.participating_objs.Add(comp);

                PropertyValueConverter.PropertyInfo m_pi = new PropertyValueConverter.PropertyInfo();
                m_pi.PropertyName = "merged_value";
                m_pi.PropertyObj = merged_pi;
                m_pi.propertyInfo = merged_pi.GetType().GetProperty("merged_value");
                m_pi.PropertyType = pi.PropertyType;
                m_pi.PropertyValue = merged_pi.merged_value;

                ppack.merged_propertyinfo = m_pi;
                ppack.participating_components = compack.merged_components;
                pack.Add(ppack);
            }
            return pack;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    public class Multi_GameObjectListToMultiCompPack : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            Dictionary<string, List<GameObject>> compToObjsDict = new Dictionary<string,List<GameObject>>();

            //build table
            var list = value as ObservableCollection<GameObject>;
            foreach (var gobj in list)
            {
                foreach (var comp in gobj.mComponents)
                {
                    string comp_name = comp.mytoplevelname();
                    if (!compToObjsDict.ContainsKey(comp_name))
                        compToObjsDict.Add(comp_name, new List<GameObject>());
                    compToObjsDict[comp_name].Add(gobj);
                }
            }

            //generate packs from the table
            var pack_list = new List<MultiComponentPack>();
            foreach (var input in compToObjsDict)
            {
                MultiComponentPack pack = new MultiComponentPack();
                pack.component_type = input.Key;
                pack.gameObjects.AddRange(input.Value);
                foreach (var gobj in input.Value)
                    pack.merged_components.Add(gobj.GetComponentByName(input.Key));
                pack_list.Add(pack);
            }

            return pack_list;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    public partial class MultiPropertiesControl : UserControl
    {
        public MultiPropertiesControl()
        {
            InitializeComponent();
        }
    }
}
