using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace MochaEditor.MCommand
{
    /*
     * Hot key and command layer extrapolation for the Mocha Engine
     */
    public class MCommand : INotifyPropertyChanged
    {
        public static Dictionary<string, MCommand> commands = new Dictionary<string,MCommand>();
        HotKey _hotkey;
        string _descriptiveName;
        string _keyName;
        public string descriptiveName { get { return _descriptiveName; } set { _descriptiveName = value; OnPropertyChanged("descriptiveName"); } }
        public string keyName { get { return _keyName; } set { _keyName = value; OnPropertyChanged("keyName"); } }
        public string hotKeyText { get; set; }

        public MCommand(HotKey h, string _descriptiveName, string _keyName)
        {
            _hotkey = h;
            descriptiveName = _descriptiveName;
            keyName = _keyName;
            hotKeyText = h.ToString();
            try
            {
                commands.Add(_keyName, this);
            }
            catch (ArgumentException)
            {
                throw new InvalidOperationException("MCommand key name already added.");
            }
        }

        public void InvokeCommand()
        {
            _hotkey.Action();
        }

        public void UnregisterCommand()
        {
            _hotkey.Unregister();
        }
        public void RegisterCommand()
        {
            _hotkey.Register();
        }
        #region PROPERTYCHANGED
        public event PropertyChangedEventHandler PropertyChanged;

        public void OnPropertyChanged(string name)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }
        #endregion
    }

    public class MCommands
    {
        public static bool _registered = false;
        public static void BindCommands()
        {
            new MCommand(new HotKey(System.Windows.Input.Key.S, KeyModifier.Alt,
                new Action(() => { EngineCommandBindings.CMD_SnapToGrid(); })),
                "Snap to grid", "SnapToGrid");
            new MCommand(new HotKey(System.Windows.Input.Key.S, KeyModifier.Ctrl,
                new Action(() => { EngineCommandBindings.CMD_SaveMap(); })),
                "Save Map", "SaveMap");
            new MCommand(new HotKey(System.Windows.Input.Key.S, KeyModifier.Ctrl | KeyModifier.Alt,
                new Action(() => { EngineCommandBindings.CMD_SaveMapAs(); })),
                "Save Map As", "SaveMapAs");
            new MCommand(new HotKey(System.Windows.Input.Key.D, KeyModifier.Ctrl,
                new Action(() => { EngineCommandBindings.CMD_DuplicateObjects(); })),
                "Duplicate object(s)", "DuplicateObjects");
            new MCommand(new HotKey(System.Windows.Input.Key.Delete, KeyModifier.Alt,
                new Action(() => { EngineCommandBindings.CMD_DeleteObjects(); })),
                "Delete object(s)", "DeleteObjects");
            new MCommand(new HotKey(System.Windows.Input.Key.L, KeyModifier.Ctrl,
                new Action(() => { EngineCommandBindings.CMD_LoadMap(); })),
                "Load Map", "LoadMap");
            new MCommand(new HotKey(System.Windows.Input.Key.M, KeyModifier.Ctrl | KeyModifier.Alt,
                new Action(() => { EngineCommandBindings.CMD_MergeMap(); })),
                "Merge Map", "MergeMap");
            new MCommand(new HotKey(Key.A, KeyModifier.Ctrl, new Action(() => { EngineCommandBindings.CMD_SelectAllObjects(); })),
                "Select All", "SelectAll");
            new MCommand(new HotKey(Key.Space, KeyModifier.Ctrl, new Action(() => { EngineCommandBindings.CMD_FindObject(); })),
                "Find Object", "FindObject");

            _registered = true;

            MochaScriptSystem.ScriptSubsystemManaged.IsRegistering += ScriptSubsystemManaged_IsRegistering;
        }

        static void ScriptSubsystemManaged_IsRegistering(object sender, EventArgs e)
        {
            MochaScriptSystem.ScriptSubsystemManaged.getInstance()._focus = RegisterAll;
            MochaScriptSystem.ScriptSubsystemManaged.getInstance()._unfocus = UnregisterAll;
        }

        public static void RegisterAll()
        {
            
            if (_registered)
                return;
            else
            {
                
                foreach(var key in MCommand.commands.Keys)
                {
                    var value = MCommand.commands[key];
                    value.RegisterCommand();
                    //Console.WriteLine(value.keyName);
                }
                Console.WriteLine("REGISTERING" + MCommand.commands.Count.ToString());
                _registered = true;
                EngineManagerViewModel.instance._isFocused = true;
            }
        }
        public static void UnregisterAll()
        {
            
            if (!_registered)
                return;
            else 
            {
                
                foreach (var key in MCommand.commands.Keys)
                {
                    var value = MCommand.commands[key];
                    value.UnregisterCommand();
                    //Console.WriteLine(value.keyName);
                }
                Console.WriteLine("UNREGISTERING" + MCommand.commands.Count.ToString());
                _registered = false;
                EngineManagerViewModel.instance._isFocused = false;
            }
        }
    }
}
