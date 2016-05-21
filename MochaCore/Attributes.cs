using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

[AttributeUsage(AttributeTargets.All)]
public class MochaDoNotShow : System.Attribute
{

    
}

[AttributeUsage(AttributeTargets.Property)]
public class MochaHint : Attribute
{
    public Type converterType;
    public object converter;

    public MochaHint(Type _converterType)
    {
        converterType = _converterType;
        converter = _converterType.GetConstructor(Type.EmptyTypes).Invoke(null);
    }

    public MochaHint(string _converterType)
    {
        converterType = Type.GetType(_converterType, true);
        converter = converterType.GetConstructor(Type.EmptyTypes).Invoke(null);
    }

}