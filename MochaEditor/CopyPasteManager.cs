using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Reflection;

public static class CopyPasteManager
{
    struct CopyPasteData {
        public string name;
        public object value;
    }

    static List<CopyPasteData> clipboard = new List<CopyPasteData>();
    static Type stored_type = null;

    public static void Copy(object c)
    {
        clipboard.Clear();
        if (c == null)
            return;

        if ((c as MochaInterface.Component).myType == "CScript")
        {
            MochaInterface.CScript cscript = MochaEditor.CastingHelper.CastTo<MochaInterface.CScript>((MochaInterface.Component)c);
            MochaScriptSystem.BoundScript bs = MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetBoundScriptFromCScript(cscript);
            stored_type = bs.scriptObject.GetType();
            foreach (var pi in bs.scriptObject.GetType().GetProperties())
            {
                if (pi.Name == "gameObject") continue;  //do not copy the gameObject this is bound to
                if (pi.GetCustomAttribute<MochaDoNotShow>() != null) continue;
                CopyPasteData pd = new CopyPasteData()
                {
                    name = pi.Name,
                    value = pi.GetValue(bs.scriptObject)
                };
                clipboard.Add(pd);
            }
            return;
        }

        stored_type = c.GetType();

        MochaEditor.PropertyValueConverter pvc = new MochaEditor.PropertyValueConverter();

        foreach (MochaEditor.PropertyValueConverter.PropertyInfo pi in (ObservableCollection<MochaEditor.PropertyValueConverter.PropertyInfo>)pvc.Convert(c, null, "Component", null))
        {
            CopyPasteData pd = new CopyPasteData() { 
                name = pi.PropertyName,
                value = pi.PropertyValue
            };
            clipboard.Add(pd);
        }
    }

    public static void Paste(object dest)
    {
        if (dest == null || stored_type == null)
            return;

        if ((dest as MochaInterface.Component).myType == "CScript")
        {
            MochaInterface.CScript cscript = MochaEditor.CastingHelper.CastTo<MochaInterface.CScript>((MochaInterface.Component)dest);
            MochaScriptSystem.BoundScript bs = MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetBoundScriptFromCScript(cscript);
            if (bs.scriptObject.GetType() != stored_type)
            {
                MochaEditor.Controls.ModernModalWindow mmw = new MochaEditor.Controls.ModernModalWindow("Copy/Paste types do not match.");
                return; 
            }

            foreach (var pi in bs.scriptObject.GetType().GetProperties())
            {
                if (pi.Name == "gameObject") continue;  //do not copy the gameObject this is bound to

                foreach (CopyPasteData data in clipboard)
                {
                    if (pi.Name == data.name && pi.PropertyType == data.value.GetType())
                    {
                        pi.SetValue(bs.scriptObject, data.value);
                    }
                }
            }
            return;
        }

        if (dest.GetType() != stored_type)
        {
            MochaEditor.Controls.ModernModalWindow mmw = new MochaEditor.Controls.ModernModalWindow("Copy/Paste types do not match.");
            return; 
        }

        MochaEditor.PropertyValueConverter pvc = new MochaEditor.PropertyValueConverter();
        var pilist = (ObservableCollection<MochaEditor.PropertyValueConverter.PropertyInfo>)pvc.Convert(dest, null, "Component", null);
        foreach (CopyPasteData data in clipboard)
        {
            foreach (MochaEditor.PropertyValueConverter.PropertyInfo pi in pilist)
            {
                if (pi.PropertyName == data.name && pi.PropertyType == data.value.GetType())
                {
                    pi.PropertyValue = data.value;
                }
            }
        }
    }
}

