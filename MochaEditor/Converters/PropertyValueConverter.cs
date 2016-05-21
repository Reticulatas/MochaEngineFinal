using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Reflection;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Globalization;

namespace MochaEditor
{
    [ValueConversion(typeof(object), typeof(ObservableCollection<PropertyInfo>))]
    public class PropertyValueConverter : IValueConverter
    {
        public class PropertyInfo
        {
            public static Dictionary<string, Type> nameToTypeLookup = null;

            public String PropertyName { get; set; }
            public object PropertyValue
            {
                get
                {
                    return propertyInfo.GetValue(PropertyObj);
                }
                set
                {
                    propertyInfo.SetValue(PropertyObj, value);
                }
            }
            public object PropertyObj { get; set; }
            public System.Reflection.PropertyInfo propertyInfo;
            public Type PropertyType { get; set; } 

            public PropertyInfo(string prop_name, Type prop_type, System.Reflection.PropertyInfo prop_info, object prop_obj)
            {
                PropertyName = prop_name;
                PropertyType = prop_type;
                propertyInfo = prop_info;
                PropertyObj = prop_obj;
            }
            public PropertyInfo() { }
        }

        public object Convert(object ivalue, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (PropertyInfo.nameToTypeLookup == null)
            {
                //first time generate name to type lookups
                PropertyInfo.nameToTypeLookup = new Dictionary<string, Type>();
                var lListOfBs = (from lAssembly in AppDomain.CurrentDomain.GetAssemblies()
                                 from lType in lAssembly.GetTypes()
                                 where typeof(MochaInterface.IMeta).IsAssignableFrom(lType)
                                 select lType).ToArray();
                foreach (Type t in lListOfBs)
                    PropertyInfo.nameToTypeLookup.Add(t.Name, t);
            }

            var retvalue = new ObservableCollection<PropertyInfo>();
            if (ivalue != null)
            {
                object value = ivalue;
                if (parameter as string == "Component")
                {
                    //cast up
                    try
                    {
                        Type t = PropertyInfo.nameToTypeLookup[(ivalue as MochaInterface.IMeta).mytoplevelname()];
                        value = CastingHelper.castTo(ivalue, t);
                    }
                    catch (KeyNotFoundException)
                    {
                        Logger.Log("Component Error: Cannot find Component Type: " + (ivalue as MochaInterface.Component).myType);
                        return null;
                    }
                    catch (NotSupportedException e)
                    {
                        Logger.Log("Component Error: Error converting: " + e.Message);
                        return null;
                    }
                    catch (InvalidCastException e)
                    {
                        Logger.Log("Component Error: Error converting: " + e.Message);
                        return null;
                    }
                }

                //get all properties
                var properties = value.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance);
                foreach (var propertyInfo in properties)
                {
                    PropertyInfo info;
                    try
                    {
                        object pvalue = propertyInfo.GetValue(value);//?? "Null";
                        info = new PropertyInfo(propertyInfo.Name, propertyInfo.PropertyType, propertyInfo, value);
                    }
                    catch (Exception )
                    {
                        info = new PropertyInfo(propertyInfo.Name, propertyInfo.PropertyType, propertyInfo, value);
                    }
                    if (propertyInfo.GetCustomAttribute(typeof(MochaDoNotShow)) == null)
                        retvalue.Add(info);
                }
                return retvalue;
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }
}
