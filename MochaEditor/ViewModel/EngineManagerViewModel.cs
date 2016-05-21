using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;
using System.Diagnostics;
using System.IO;
using MochaInterface;
using System.Windows;

namespace MochaEditor
{
    /*
     * This class is actually a singleton - MVVM failed me
     * */
    class EngineManagerViewModel : INotifyPropertyChanged
    {
        public static EngineManagerViewModel instance = null;
        public bool _isFocused = false;
        const int REFRESHTIME = 1;

        public static bool isApplicationActive;

        Models.EngineManagerModel _model;

        public EngineManagerViewModel()
        {
            instance = this;
            _model = new Models.EngineManagerModel();

            ComponentTypes = new ObservableCollection<String>();

        }

        string _busymessage = null;
        public string BusyMessage
        {
            get { return _busymessage; }
            set { _busymessage = value; OnPropertyChanged("BusyMessage");
                if (_busymessage != null)
                {
                    //show engine
                   System.Windows.Application.Current.Dispatcher.Invoke(() => _model.EngineEmbedControl.SetVisible(true));
                }
                else
                {
                    //hide engine
                    System.Windows.Application.Current.Dispatcher.Invoke(() => _model.EngineEmbedControl.SetVisible(false));
                }
            }
        }

        public bool EngineEmbedHasVisibility { get; set; }

        public bool IsConnected { get { return _model.IsConnected; } set { _model.IsConnected = value; OnPropertyChanged("IsConnected"); } }
        public string EngineStatusText { get { return _model.EngineStatusText; } }
        public ObservableCollection<String> ComponentTypes { get; set; }
        public string[] ComponentTypesList = new string[] {"Hello"};

        public Containers.SuppressableObservableCollection<GameObject> SelectedGameObjects
        {
            get
            {
                return _model._selectedGameObjects;
            }
            set
            {
                _model._selectedGameObjects = value;
                OnPropertyChanged("SelectedGameObjects");
            }
        }

        public void ForceSelectedGameObjectRefresh()
        {
            List<GameObject> v = SelectedGameObjects.ToList();
            SelectedGameObjects.Clear(false);
            SelectedGameObjects.AddRange(v);
            OnPropertyChanged("SelectedGameObjects");
        }

        //does not call SetSelected on change (for model)
        public void ForceSetSelectedGameObject(GameObject g)
        {
            EngineManagerViewModel.instance.SelectedGameObjects.Replace(g);
            OnPropertyChanged("SelectedGameObjects");
        }

        public void ForceRefreshGameObjects()
        {
            _previousGameObjects = 0;
            Containers.SuppressableObservableCollection<GameObject> gs = CurrentGameObjects;
            CurrentGameObjects.Clear(false);// = null;
            OnPropertyChanged("NumberOfObjects");
            CurrentGameObjects.AddRange(CurrentGameObjects);
            OnPropertyChanged("NumberOfObjects");
        }


        string _currentLoadedMap;
        public String currentLoadedMap { get { return _currentLoadedMap; } set { _currentLoadedMap = value; PropertyChanged(this, new PropertyChangedEventArgs("currentLoadedMap")); } }

        public void Connect()
        {
            if (_model.EngineEmbedControl == null)
            {
                Logger.Log("MInt Error: must set AppControl to embed");
                return;
            }

            Thread thread = new Thread(new ThreadStart(_model.T_EngineThread));
            thread.Start();
            Thread.Sleep(10);
            while (Engine.getInstance().GetIsInited() == false)
              Thread.Sleep(1);
           
            //prep engine for editor mode
            Engine.getInstance().RegisterEditorMode();
            //EditorSubsystemManaged.getInstance().RegisterTick();

            _model.EngineEmbedControl.Recreate();

            //generate component type list
            ComponentTypes.Clear();
            var lListOfBs = (from lAssembly in AppDomain.CurrentDomain.GetAssemblies()
                             from lType in lAssembly.GetTypes()
                             where typeof(MochaInterface.Component).IsAssignableFrom(lType)
                             select lType).ToList();
            foreach (Type t in lListOfBs)
                if (t != typeof(MochaInterface.Component))
                    ComponentTypes.Add(t.Name);
            
            //run roaster
            Thread roasterThread = new Thread(new ThreadStart(() =>
            {
                while (true)
                {
                    if (EngineManagerViewModel.instance.BusyMessage == null)    //don't run roaster if another operation is pending
                    {
                        //start up the roaster, wait for it to idle, output it's info to the log
                        ProcessStartInfo startInfo = new ProcessStartInfo();
                        startInfo.FileName = "Roaster.exe";
                        startInfo.WindowStyle = ProcessWindowStyle.Hidden;
                        startInfo.CreateNoWindow = true;
                        startInfo.UseShellExecute = false;
                        startInfo.RedirectStandardOutput = true;
                        try
                        {
                            using (Process roastProc = System.Diagnostics.Process.Start(startInfo))
                            {
                                Thread.Sleep(1000);
                                if (roastProc.HasExited == false)
                                {
                                    EngineManagerViewModel.instance.BusyMessage = "... Roasting New Assets ...";
                                    roastProc.WaitForExit();
                                    EngineManagerViewModel.instance.BusyMessage = null;
                                }
                                using (StreamReader reader = roastProc.StandardOutput)
                                {
                                    string result = reader.ReadToEnd();
                                    if (!String.IsNullOrWhiteSpace(result))
                                        Logger.Log(result);
                                }

                            }
                        }
                        catch (Win32Exception)
                        {
                            //roaster was not found
                            Console.WriteLine("Roaster.exe not found - assets will not be reloaded.");
                            Thread.CurrentThread.Abort();
                        }
                    }
                    Thread.Sleep(ROASTERINTERVAL);
                }
            }));
            roasterThread.Start();

            IsConnected = true;
        }

        #region BINDTARGETS

        private DispatcherTimer _currentGameObjects_Timer;
        private void NotifyChange_CurrentGameObjects(object o, EventArgs e)
        {            
            if (!IsConnected || GSM.getInstance().getActiveState() == null)
                _model._currentGameObjects.Clear();
            else
            {
                EditorSubsystemManaged.getInstance().QueueAction(new Action(()=> {
                    uint objs = GSM.getInstance().getActiveState().GetManagedObjectsSlotTable().GetNumberOfObjects();
                    //we're also going to check the first id in the list as a sort-of secondary measure in case the entire list has been dup'ed
                    IntPtr firstIDiter = GSM.getInstance().getActiveState().GetManagedObjectsSlotTable().GetAllocatedObjects().create_iterator_begin();
                    uint firstID = GSM.getInstance().getActiveState().GetManagedObjectsSlotTable().GetAllocatedObjects().get_next_key(firstIDiter).GetID();
                    if ((objs != _previousGameObjects || (_model._currentGameObjects.Count != 0) &&
                        (objs != 0 && firstID != _model._currentGameObjects.ElementAt(0).GetID())))
                    {
                        //rebuild
                        List<GameObject> newItems = new List<GameObject>();
                        GameObjectIterator iter = new GameObjectIterator(GSM.getInstance().getActiveState());
                        while (true)
                        {
                            GameObject g = GameObject.From(iter.Next());
                            if (g == null)
                                break;
                            newItems.Add(g);
                        }

                        _previousGameObjects = objs;

                        //change ui
                        Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            _model._currentGameObjects.Clear(false);
                            _model._currentGameObjects.AddRange(newItems);
                            OnPropertyChanged("NumberOfObjects");
                        }));
                    }
                }));
            }
        }
        public uint NumberOfObjects
        {
            get
            {
                return (uint)_model._currentGameObjects.Count;
            }
        }

        private uint _previousGameObjects = 0;
        public Containers.SuppressableObservableCollection<GameObject> CurrentGameObjects
        {
            get
            {
                if (_currentGameObjects_Timer == null)
                {
                    _currentGameObjects_Timer = new DispatcherTimer(DispatcherPriority.ApplicationIdle);
                    _currentGameObjects_Timer.Tick += new EventHandler(NotifyChange_CurrentGameObjects);
                    _currentGameObjects_Timer.Interval = new TimeSpan(0, 0, REFRESHTIME);
                    _currentGameObjects_Timer.Start();
                }
                return _model._currentGameObjects;
            }
        }

        #endregion

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

        public TimeSpan ROASTERINTERVAL = new TimeSpan(0, 0, 1);
    }
}
