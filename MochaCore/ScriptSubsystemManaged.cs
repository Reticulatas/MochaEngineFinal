using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Threading;
using System.Threading;
using System.Globalization;
using System.Diagnostics;
using Microsoft.Xna.Framework;

namespace MochaScriptSystem 
{
    //represents an event that will happen to the script before it's next update
    public struct ScriptEvent
    {
        public string methodname;
        public object data;
    }

    //stores runtime info about a script, including the actual compiled assembly object
    public class BoundScript
    {
        public string register_id;                //corresponds to the register id on the script object in CScript (for serialization)
        public object scriptObject { get; set; }
        public string scriptName;
        public MochaInterface.CScript scriptComponent;
        public MethodInfo Func_OnUpdate;
        public bool hasCalledOnStart = false;
        public Queue<ScriptEvent> scriptEvents = new Queue<ScriptEvent>();
    }

    //Storage class used by SerializeScriptData that contains property info data for serialization
    public class SerializeScriptPropertyData
    {
        public object property_value;
        public string property_name;
        public string property_type_name;

        //Wrapper class used to store nullptrs in case they are serialized
        [Serializable]
        public class SerializeScriptValueWrapper
        {
            public object property_value;
        }
        [Serializable]
        public class SerializeScriptValueCustomWrapper
        {
            public object property_value;
        }

        //add custom wrappers here - they will automatically be used if the type was
        //not naively serializable
        public class SerializeCustomWrappers
        {
            public static object WrapSave_Vector3(object v)
            {
                Vector3 ve = (Vector3)v;
                return new float[3] { ve.X, ve.Y, ve.Z };
            }
            public static object WrapLoad_Vector3(object v)
            {
                float[] comps = (float[])v;
                return new Vector3(comps[0], comps[1], comps[2]);
            }
        }
    }

    //Pre-cursor class that stores a serialized script data until it is requested by the bindor
    public class SerializedScriptData
    {
        public List<SerializeScriptPropertyData> property_store = new List<SerializeScriptPropertyData>();
        public string register_id;

        public SerializedScriptData(string _id) { register_id = _id; }
        public SerializedScriptData() { register_id = ""; }
    }

    public class ScriptSubsystemManaged
    {
        public HashSet<BoundScript> registeredScripts = new HashSet<BoundScript>();
        public Dictionary<MochaInterface.CScript, BoundScript> cscriptToBoundScriptLookup = new Dictionary<MochaInterface.CScript, BoundScript>();
        public Dictionary<string, SerializedScriptData> serializedScriptsDatabase = new Dictionary<string, SerializedScriptData>();
        public Dictionary<MochaInterface.GameObject, List<BoundScript>> gameObjectToBoundScriptsLookup = new Dictionary<MochaInterface.GameObject, List<BoundScript>>();
        bool hasErrors = false;
        public bool HasErrors { get { return hasErrors; } set { hasErrors = value; } }
        public static event EventHandler HasReloaded;
        public static event EventHandler IsReloading;
        public static event EventHandler IsRegistering;

        ~ScriptSubsystemManaged() { }

        public void SetHasError(bool _noErrors)
        {
            hasErrors = _noErrors;
        }

        public bool GetHasErrors()
        {
            return hasErrors;
        }

        /// Safely copies to bound scripts, keeping track of removed or added variables
        void CopyObject(object from, object to)
        {
            foreach (PropertyInfo pi in from.GetType().GetProperties())
            {
                if (pi.Name == "gameObject") continue;  //do not copy the gameObject this is bound to
                PropertyInfo to_pi = to.GetType().GetProperty(pi.Name);
                if (to_pi == null)
                    continue;           //To object does not contain variable in From
                if (to_pi.CanWrite == false)
                    continue;
                if (to_pi.PropertyType != pi.PropertyType)  //object types do not match
                    continue;
                to_pi.SetValue(to, pi.GetValue(from));
            }
        }

        //redirect from the copy constructor in c++, binds and injects the values
        void CopyFrom(IntPtr _from, IntPtr _to)
        {
            MochaInterface.CScript to = new MochaInterface.CScript(_to, false);
            MochaInterface.CScript from = new MochaInterface.CScript(_from, false);

            //copy properties over
            object to_sc = cscriptToBoundScriptLookup[to].scriptObject;
            object from_sc = null;
            if (cscriptToBoundScriptLookup.ContainsKey(from))
            {
                from_sc = cscriptToBoundScriptLookup[from].scriptObject;
                CopyObject(from_sc, to_sc);

                if (from.gameObject.GetIsDormantObject())
                {
                    //HACK: remove this object is it came from a prefab - it has served its purpose
                    _unregister(_from);
                }

            }
            else
                Logger.Log("WARNING: No defaults values will be copied for object from prefab: " + to.gameObject.GetName());
        }

        void QueueAction(IntPtr cs_ptr, string _methodname, IntPtr data, string data_type)
        {
            object data_obj = null;
            if (data != null)
            {
                MochaInterface.IMeta imeta_obj = new MochaInterface.IMeta(data, false);
                if (String.IsNullOrWhiteSpace(data_type))
                    Logger.Log("ERROR: Failed to convert queued event parameter because data_type is null");
                else
                    data_obj = CastingHelper.castTo(imeta_obj, Type.GetType("MochaInterface."+data_type));
            }
            MochaInterface.CScript cs = new MochaInterface.CScript(cs_ptr, false);
            BoundScript bs = cscriptToBoundScriptLookup[cs];
            bs.scriptEvents.Enqueue(new ScriptEvent() { methodname = _methodname, data = data_obj });
        }

        /// <summary>
        /// Reload all scripts, rebind objects, and force compilation
        /// </summary>
        public void Reload()
        {
            if (IsReloading != null)
                IsReloading(this, null);

            ScriptManager.getInstance().CompileAll();

            foreach (BoundScript bs in registeredScripts)
            {
                if (bs.scriptObject != null)
                {
                    object newscriptObject = ScriptManager.getInstance().MakeObject(bs.scriptObject.GetType().Name);

                    //do property copy
                    CopyObject(bs.scriptObject, newscriptObject);

                    bs.scriptObject = newscriptObject;
                }
            }

            if (HasReloaded != null)
                HasReloaded(this, null);

            Console.WriteLine("...Reloaded all scripts.");
        }

        bool IsPropertyWrapped(string propertyType)
        {
            MethodInfo wrapper_func = typeof(SerializeScriptPropertyData.SerializeCustomWrappers).GetMethod("WrapSave_" + propertyType);
            return (wrapper_func != null);
        }
        MethodInfo GetPropertyWrappedFunc(string propertyType)
        {
            MethodInfo wrapper_func = typeof(SerializeScriptPropertyData.SerializeCustomWrappers).GetMethod("WrapSave_" + propertyType);
            return wrapper_func;
        }

        void Serialize()
        {
            BinaryFormatter bf = new BinaryFormatter();
            using (FileStream fs = new FileStream(Path.GetTempPath() + "temp\\scriptdata.flavor", FileMode.Create))
            {
                foreach (BoundScript bs in registeredScripts)
                {
                    if (String.IsNullOrWhiteSpace(bs.register_id))
                    {
                        Logger.Log("ELROY ERROR 2: THE ELROYENING: NO REGISTER ID SET FOR BOUNDSCRIPT");
                        continue;
                    }
                    //write header
                    bf.Serialize(fs, bs.register_id);
                    if (bs.scriptObject != null)
                    {
                        PropertyInfo[] properties = bs.scriptObject.GetType().GetProperties();
                        bf.Serialize(fs, properties.Count((PropertyInfo pi) => { return pi.CanWrite && (pi.PropertyType.IsSerializable || IsPropertyWrapped(pi.PropertyType.Name)); }));
                        //write all properties
                        foreach (PropertyInfo property in properties)
                        {
                            if (property.CanWrite)
                            {
                                if (property.PropertyType.IsSerializable)
                                {
                                    //non wrapped value
                                    bf.Serialize(fs, property.Name);                        //property name
                                    bf.Serialize(fs, property.PropertyType.Name);           //property type name
                                    object value = property.GetValue(bs.scriptObject);
                                    SerializeScriptPropertyData.SerializeScriptValueWrapper wrapper = new SerializeScriptPropertyData.SerializeScriptValueWrapper();
                                    wrapper.property_value = value;
                                    bf.Serialize(fs, wrapper);
                                }
                                else
                                {
                                    MethodInfo wrapper_func = GetPropertyWrappedFunc(property.PropertyType.Name);
                                    if (wrapper_func != null)
                                    {
                                        //custom serialize wrapped value
                                        bf.Serialize(fs, property.Name);
                                        bf.Serialize(fs, property.PropertyType.Name);
                                        object value = property.GetValue(bs.scriptObject);
                                        SerializeScriptPropertyData.SerializeScriptValueCustomWrapper wrapper = new SerializeScriptPropertyData.SerializeScriptValueCustomWrapper();
                                        wrapper.property_value = wrapper_func.Invoke(null, new object[] { value });
                                        bf.Serialize(fs, wrapper);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        SetHasError(true);
                        bf.Serialize(fs, (int)0);
                        Logger.Log("No property info serialized for malformed script " + bs.register_id);
                    }
                }
                string eof_id = "";
                bf.Serialize(fs, eof_id);
            }
        }

        void DeSerialize()
        {
            string scriptdata_filename = Path.GetTempPath() + "temp\\scriptdata.flavor";
            FileStream fs = null;
            try
            {
                fs = new FileStream(scriptdata_filename, FileMode.Open);
            }
            catch (Exception e)
            {
                Console.WriteLine("ERROR: Failed to open " + scriptdata_filename + " no scripts will be serialized! " + e.Message);
                throw e;
            }
            BinaryFormatter bf = new BinaryFormatter();
            serializedScriptsDatabase.Clear();

            try
            {
                while (true)
                {
                    if (fs.Position >= fs.Length)   //eof register
                        break;
                    string register_id = (string)bf.Deserialize(fs);

                    SerializedScriptData ssd = new SerializedScriptData(register_id);
                    int number_of_props = (int)bf.Deserialize(fs);
                    for (int i = 0; i < number_of_props; ++i)
                    {
                        SerializeScriptPropertyData script_data = new SerializeScriptPropertyData();
                        script_data.property_name = (string)bf.Deserialize(fs);
                        script_data.property_type_name = (string)bf.Deserialize(fs);
                        object deserwrapper = bf.Deserialize(fs);
                        if (deserwrapper is SerializeScriptPropertyData.SerializeScriptValueWrapper)
                        {
                            //support new format without wrapped loading
                            SerializeScriptPropertyData.SerializeScriptValueWrapper wrapper = (SerializeScriptPropertyData.SerializeScriptValueWrapper)deserwrapper;
                            script_data.property_value = wrapper.property_value;
                        }
                        else if (deserwrapper is SerializeScriptPropertyData.SerializeScriptValueCustomWrapper)
                        {
                            //custom wrapped load
                            SerializeScriptPropertyData.SerializeScriptValueCustomWrapper wrapper = (SerializeScriptPropertyData.SerializeScriptValueCustomWrapper)deserwrapper;
                            MethodInfo load_wrapper = typeof(SerializeScriptPropertyData.SerializeCustomWrappers).GetMethod("WrapLoad_" + script_data.property_type_name);
                            if (load_wrapper == null)
                            {
                                Logger.Log("Attempt to load: " + script_data.property_name + " but no longer has wrapped load function.");
                                continue;
                            }
                            script_data.property_value = load_wrapper.Invoke(null, new object[] { wrapper.property_value });
                        }
                        else
                            script_data.property_value = deserwrapper;
                        ssd.property_store.Add(script_data);
                    }
                    try
                    {
                        serializedScriptsDatabase.Add(register_id, ssd);
                    }
                    catch (Exception)
                    {
                        Logger.Log("Repeated Script Register ID found: " + register_id);
                    }
                }
            }
            catch (System.Runtime.Serialization.SerializationException e)
            {
                Console.WriteLine("ERROR: Failed to serialize " + scriptdata_filename + " some scripts will NOT be serialized! " + e.Message);
            }
            catch (Exception e)
            {
                Console.WriteLine("ERROR: Failed to serialize " + scriptdata_filename + " some scripts will NOT be serialized! " + e.Message);
            }

            fs.Close();
        }

        //register self to be called by the barrista
        NativeMochaDotNetCallback _tick;
        NativeMochaDotNetCScriptCopyCallback _copyFrom;
        NativeMochaDotNetCScriptEventCallback _eventCallback;
        NativeMochaDotNetCallback _reload;
        NativeMochaDotNetCallback _serialize;
        NativeMochaDotNetCallback _deserialize;
        NativeMochaDotNetCScriptCallback _register;
        NativeMochaDotNetCScriptCallback _unregister;
        public NativeMochaDotNetCallback _focus;
        public NativeMochaDotNetCallback _unfocus;
        public void RegisterToNative()
        {
            _tick = Tick;
            _copyFrom = CopyFrom;
            _eventCallback = QueueAction;
            _reload = Reload;
            _serialize = Serialize;
            _deserialize = DeSerialize;
            _register = RegisterCScript;
            _unregister = UnRegisterCScript;
            inst = this;
            if (IsRegistering != null)
                IsRegistering(this, null);
            MochaInterface.ScriptSubsystem.getInstance().Register_Callbacks(_tick, _copyFrom, _eventCallback, _reload, 
                _serialize, _deserialize, _register, _unregister,_focus,_unfocus);
            
        }

        private void UnRegisterCScript(IntPtr from)
        {
            MochaInterface.CScript cs = new MochaInterface.CScript(from, false);

            BoundScript bs = null;
            try
            {
                bs = cscriptToBoundScriptLookup[cs];
            }
            catch (KeyNotFoundException)
            {
                Logger.Log("ERROR: Unregister skipped (not found) for script from GameObject: " + cs.gameObject.GetName());
                return;
            }

            //call OnEnd
            if (bs.hasCalledOnStart)
            {
                MethodInfo mi = bs.scriptObject.GetType().GetMethod("OnEnd");
                if (mi != null)
                {
                    try
                    {
                        mi.Invoke(bs.scriptObject, null);
                    }
                    catch (Exception e)
                    {
                        Logger.Log("WARNING: Could not call OnEnd on script: " + bs.scriptName + " because " + e.Message);
                    }
                }
            }

            registeredScripts.Remove(bs);
            cscriptToBoundScriptLookup.Remove(cs);
            var bslist = gameObjectToBoundScriptsLookup[cs.gameObject];
            bslist.Remove(bs);
            if (bslist.Count == 0)
                gameObjectToBoundScriptsLookup.Remove(cs.gameObject);
        }

        public void ForceLinkAllScriptProperties()
        {
            //rewrite scripts to their stored defaults (if there are any)
            foreach (BoundScript bs in registeredScripts)
            {
                if (!RelinkScriptPropertiesFromStore(bs))
                {
                    //this bs was not found in the store, tell the user
                    Logger.Log(bs.scriptName + " does not have serialized defaults in the store. Nothing will be copied.");
                }
            }
        }

        private bool RelinkScriptPropertiesFromStore(BoundScript bs)
        {
            //serialization
            if (serializedScriptsDatabase.ContainsKey(bs.register_id))
            {
                bool error_on_register = false;
                //we have serialization data waiting for this object!
                foreach (SerializeScriptPropertyData prop_data in serializedScriptsDatabase[bs.register_id].property_store)
                {
                    if (bs.scriptObject == null)
                    {
                        //malformed script - we will lose this serialization data
                        error_on_register = true;
                        continue;
                    }
                    PropertyInfo pi = bs.scriptObject.GetType().GetProperty(prop_data.property_name);
                    if (pi == null)
                    {
                        Logger.Log("Warning: Property " + prop_data.property_name + " no longer exists on: " + bs.scriptObject.GetType().Name);
                        continue;
                    }
                    pi.SetValue(bs.scriptObject, prop_data.property_value);
                }

                if (error_on_register)
                {
                    //errors occurred during script load
                    System.Windows.Forms.MessageBox.Show("Scripts have errors, check console for more information.\nYOU WILL LOSE SERIALIZED DEFAULTS!\nCheck Console, fix errors in external program, then restart.");
                }
                return true;
            }
            return false;
        }

        private void RegisterCScript(IntPtr from)
        {
            //setup
            MochaInterface.CScript cs = new MochaInterface.CScript(from, false);
            BoundScript bs = new BoundScript();
            bs.scriptComponent = cs;
            string sc = cs.scriptAsset.GetName();       //assumed valid at this point
            sc = sc.Remove(sc.Length - 3);
            bs.scriptName = sc;
            
            //post-compilation
            bs.scriptObject = ScriptManager.getInstance().MakeObject(bs.scriptName);
            if (bs.scriptObject != null)
            {
                bs.scriptObject.GetType().GetProperty("gameObject").SetValue(bs.scriptObject, cs.gameObject, null);
                try {
                    bs.Func_OnUpdate = bs.scriptObject.GetType().GetMethod("OnUpdate");
                    if (bs.Func_OnUpdate == null)
                        throw new Exception("No public OnUpdate specified for class: " + bs.scriptName);
                }
                catch (Exception e)
                {
                    Logger.Log("ERROR compiling scripts: " + e.Message);
                }
            }
            bs.register_id = cs.register_id;

            RelinkScriptPropertiesFromStore(bs);

            bs.scriptEvents.Enqueue(new ScriptEvent() { methodname = "OnStart" });

            //indexing
            registeredScripts.Add(bs);
            cscriptToBoundScriptLookup.Add(cs, bs);
            if (!gameObjectToBoundScriptsLookup.ContainsKey(cs.gameObject))
                gameObjectToBoundScriptsLookup.Add(cs.gameObject, new List<BoundScript>());
            gameObjectToBoundScriptsLookup[cs.gameObject].Add(bs);

        }

        public void Tick()
        {
            List<BoundScript> scriptsToUpdateThisFrame = registeredScripts.ToList();

            foreach (BoundScript bs in scriptsToUpdateThisFrame)
            {
                //if the scripts have errors, stop the execution here
                if (hasErrors)
                    continue;

                //we need the scripts bound in editor mode, but we don't want them run
                if (MochaInterface.Engine.getInstance().GetIsEditorMode() && !MochaInterface.EditorSubsystem.getInstance().IsTesting())
                    continue;

                if (!bs.scriptComponent.globalEnabled())
                    continue;

                if (!bs.scriptComponent.isBinded)
                    bs.scriptComponent.isBinded = true;

                bool runUpdate = true;          //toggle off to kill the update for this frame
                //do events
                try
                {
                    while (bs.scriptEvents.Count != 0)
                    {
                        //proc event
                        ScriptEvent sevent = bs.scriptEvents.Dequeue();
                        MethodInfo sevent_mi = bs.scriptObject.GetType().GetMethod(sevent.methodname);
                        object[] parms = { };
                        if (sevent_mi != null)
                        {
                            if (sevent.data != null)
                                parms = new object[] { sevent.data };
                            sevent_mi.Invoke(bs.scriptObject, parms);
                            if (sevent.methodname == "OnStart")
                            {
                                runUpdate = false;              //skip frame 0 update when on starts are running
                                bs.hasCalledOnStart = true;     //OnEnd will not be called if OnStart was not (for intermediate copies)
                            }
                        }
                    }
                    //call OnUpdate
                    if (runUpdate)
                        if (bs.Func_OnUpdate != null)
                            bs.Func_OnUpdate.Invoke(bs.scriptObject, null);
                }
                catch (Exception e)
                {
                    HandleScriptError(e, bs);
                }
            }
        }

        void HandleScriptError(Exception e, BoundScript script)
        {
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;     //hack fix for kai's Chinese

            string err_out = "";
            err_out += "SCRIPT ERROR(" + script.scriptName + ")" + e.InnerException.Message + "\n";

            // Get stack trace for the exception with source file information
            var st = new StackTrace(e.InnerException, true);
            for (int i = 0; i < st.FrameCount; ++i)
            {
                var frame = st.GetFrame(i);
                var line = frame.GetFileLineNumber();
                err_out += "\t(" + Path.GetFileNameWithoutExtension(frame.GetFileName()) + "," + line.ToString() + ") " + frame.GetMethod() + "\n";
            }
            Logger.Log(err_out);

            Thread.CurrentThread.CurrentUICulture = CultureInfo.InvariantCulture;
        }

        //to all scripts on game object
        public void Invoke(MochaInterface.GameObject g, string funcname, bool requireReceiver)
        {
            if (g == null)
                throw new ScriptException("Invoke on null object for function: " + funcname);
            if (gameObjectToBoundScriptsLookup.ContainsKey(g))
            {
                var bslist = gameObjectToBoundScriptsLookup[g];
                foreach (BoundScript bs in bslist)
                {
                    MethodInfo mi;
                    try
                    {
                        mi = bs.scriptObject.GetType().GetMethod(funcname);
                    }
                    catch (AmbiguousMatchException)
                    {
                        Logger.Log("Ambiguous invoke for function: " + funcname + " on script target: " + bs.scriptObject.GetType().Name + " on obj: " + g.GetName());
                        return;
                    }
                    catch (ArgumentNullException)
                    {
                        Logger.Log("Invalid function name invoke for function: " + funcname + " on script target: " + bs.scriptObject.GetType().Name + " on obj: " + g.GetName());
                        return;
                    }
                    if (mi == null)
                    {
                        if (requireReceiver)
                            Logger.Log("Function to Invoke not found: function: " + funcname + " on script target: " + bs.scriptObject.GetType().Name + " on obj: " + g.GetName());
                        return;
                    }

                    try
                    {
                        mi.Invoke(bs.scriptObject, null);
                    }
                    catch (Exception e)
                    {
                        Logger.Log("Invoke failed during invoke on function: " + funcname + " on script target: " +
                            bs.scriptObject.GetType().Name + " on obj: " + g.GetName() + " with msg: [" + e.Message + "]");
                    }
                }
            }
        }

        public object GetScript(MochaInterface.GameObject g, string scriptname)
        {
            if (g == null)
            {
                Logger.Log("Null GameObject passed on invoke to: " + scriptname);
                return null;
            }
            if (gameObjectToBoundScriptsLookup.ContainsKey(g))
            {
                var bslist = gameObjectToBoundScriptsLookup[g];
                var bs = bslist.Find(x => x.scriptName == scriptname);
                if (bs == null) {
                    Logger.Log("No Script found for object: " + g.GetName() + " while looking for: " + scriptname);
                    return null;
                }
                return bs.scriptObject;

            }
            return null;
        }

        public object GetScriptClass<T>(MochaInterface.GameObject g)
        {
            if (g == null)
            {
                Logger.Log("Null GameObject passed on invoke class to find: ");
                return null;
            }
            if (gameObjectToBoundScriptsLookup.ContainsKey(g))
            {
                var bslist = gameObjectToBoundScriptsLookup[g];
                foreach (var bs in bslist)
                {
                    if (bs == null)
                        continue;
                    object c = null;
                    try
                    {
                        c = (T)bs.scriptObject;
                    }
                    catch (InvalidCastException)
                    {
                        c = null;   
                    }
                    if (c != null)
                        return bs.scriptObject;
                }
            }
            return null;
        }

        public List<T> GetScripts<T>(MochaInterface.GameObject g)
        {
                if (g == null)
                {
                    Logger.Log("Null GameObject passed on invoke class to find: ");
                    return null;
                }

                List<T> scripts = new List<T>();
                if (gameObjectToBoundScriptsLookup.ContainsKey(g))
                {
                    var bslist = gameObjectToBoundScriptsLookup[g];
                    foreach (var bs in bslist)
                    {
                        if (bs == null)
                            continue;
                        T c = default(T);
                        try
                        {
                            c = (T)bs.scriptObject;
                        }
                        catch (InvalidCastException)
                        {
                            c = default(T);
                        }
                        if (c != null)
                            scripts.Add(c);
                    }
                }
                return scripts; 
        }

        public List<T> GetScriptsInChildren<T>(MochaInterface.GameObject g)
        {
            if (g == null)
            {
                Logger.Log("Null GameObject passed on invoke class to find: ");
                return null;
            }
            List<T> scripts = new List<T>();

            foreach (MochaInterface.CTransform ct in g.transform.children)
            {
                if (ScriptSubsystemManaged.getInstance().gameObjectToBoundScriptsLookup.ContainsKey(ct.gameObject))
                {
                    foreach (BoundScript bs in ScriptSubsystemManaged.getInstance().gameObjectToBoundScriptsLookup[ct.gameObject])
                    {
                        if (bs == null || bs.scriptObject == null)
                            continue;
                        if (bs.scriptObject.GetType() == typeof(T))
                            scripts.Add((T)bs.scriptObject);
                    }
                }
            }

            return scripts;
        }

        public bool IsCScriptRegistered(object cs)
        {
            return cscriptToBoundScriptLookup.ContainsKey(cs as MochaInterface.CScript);
        }

        public BoundScript GetBoundScriptFromCScript(object cs)
        {
            return cscriptToBoundScriptLookup[cs as MochaInterface.CScript];
        }

        ///////////////////////////////////////////////////////////
        static ScriptSubsystemManaged inst = null;
        public static ScriptSubsystemManaged getInstance()
        {
            if (inst == null)
                throw new InvalidOperationException("Something is accessing scriptsubsystemmanaged before it has been registered to barrista");
            return inst;
        }

    }
}
