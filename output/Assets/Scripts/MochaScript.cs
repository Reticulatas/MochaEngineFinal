using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MochaScript
{
    [MochaDoNotShow]
    public GameObject gameObject { get; set; }

    public void Invoke(GameObject g, string funcname, bool requirereceiver = true)
    {
        MochaScriptSystem.ScriptSubsystemManaged.getInstance().Invoke(g, funcname, requirereceiver);
    }

    public T GetScriptClass<T>(GameObject g, bool logonerror = true) where T : MochaScript
    {
        T ret = MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetScriptClass<T>(g) as T;
        if (g == null)
            Logger.Log("Could not find script " + typeof(T).Name + ", null object passed");
        if (ret == null && g != null && logonerror)
            Logger.Log("Could not find script " + typeof(T).Name + " on object " + g.GetName());
        return ret;
    }

    public List<T> GetScripts<T>(GameObject g, bool logonerror = true) where T : MochaScript
    {
        List<T> ret = MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetScripts<T>(g);
        if (g == null)
            Logger.Log("Could not find scripts " + typeof(T).Name + ", null object passed");
        if (ret == null && g != null && logonerror)
            Logger.Log("Could not find scripts " + typeof(T).Name + " on object " + g.GetName());
        return ret;
    }

    public static T GetScript<T>(GameObject g, bool logonerror = true) where T : MochaScript
    {
        T ret = MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetScript(g, typeof(T).Name) as T;
        if (g == null)
            Logger.Log("Could not find script " + typeof(T).Name + ", null object passed");
        if (ret == null && g != null && logonerror)
            Logger.Log("Could not find script " + typeof(T).Name + " on object " + g.GetName());
        return ret;
    }

    public List<T> GetScriptsInChildren<T>(MochaInterface.GameObject g) where T : MochaScript
    {
        List<T> ret = MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetScriptsInChildren<T>(g);
        if (g == null)
            Logger.Log("Could not find scripts " + typeof(T).Name + ", null object passed");
        if (ret == null && g != null)
            Logger.Log("Could not find scripts " + typeof(T).Name + " on object " + g.GetName());
        return ret;
    }

    /*
     public void AddScript<T>(MochaInterface.GameObject g) where T : MochaScript
     {
         CScript cs = CastingHelper.CastTo<CScript>(g.RequireComponentByName("CScript"));
         cs.scriptAsset = AssetManager.getInstance().GetAsset(typeof(T).Name);
     }*/
}
