using System;
using System.Collections.Generic;

namespace MochaEditor
{
    public class MultiPropertyInfoWrapper
    {
        public string merged_name { get; set; }
        object _merged_value;
        public object merged_value { get { return _merged_value; } set {
            _merged_value = value;
            if (participating_objs != null)
                foreach (var obj in participating_objs)
                    obj.GetType().GetProperty(merged_name).SetValue(obj, _merged_value);
        } }
        public List<object> participating_objs { get; set; }
    }

    public class MultiComponentPack
    {
        public List<MochaInterface.Component> merged_components = new List<MochaInterface.Component>();  //one type of component
        public List<MochaInterface.GameObject> gameObjects = new List<MochaInterface.GameObject>();       //list of objects participating (for deeper binds)

        public string component_type { get; set; }              //what type this component is
    }

    public class MultiPropertyPack
    {
        List<MochaInterface.Component> _participating_components = new List<MochaInterface.Component>();
        public List<MochaInterface.Component> participating_components
        {
            get { return _participating_components; }
            set
            {
                _participating_components = value;
            }
        }
        public MochaEditor.PropertyValueConverter.PropertyInfo merged_propertyinfo { get; set;}

        public object merged_data {get; set;}       //'fake' data container representing current state of all data
    }

}