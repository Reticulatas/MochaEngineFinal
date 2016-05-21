using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Threading;

//public delegate void NativeMochaDotNetCallback();

namespace MochaEditor.Models
{
    public class EngineManagerModel : INotifyPropertyChanged
    {

        public bool IsConnected { get; set; }
        public string EngineStatusText { get; private set; }
        public EngineControl EngineEmbedControl { get; set; }

        public Containers.SuppressableObservableCollection<GameObject> _currentGameObjects = new Containers.SuppressableObservableCollection<GameObject>();

        Containers.SuppressableObservableCollection<GameObject> _selectedGameObjects_ = new Containers.SuppressableObservableCollection<GameObject>();
        public Containers.SuppressableObservableCollection<GameObject> _selectedGameObjects
        {
            get
            {
                return _selectedGameObjects_;
            }
            set
            {
                _selectedGameObjects_ = value;
                OnPropertyChanged("_selectedGameObjects");
            }
        }

        public static EngineManagerModel instance = null;

        public EngineManagerModel()
        {
            instance = this;
            _selectedGameObjects.CollectionChanged += _selectedGameObjects_CollectionChanged;
        }

        void _selectedGameObjects_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            //unselect everything first
            /*MochaInterface.EditorSubsystem.getInstance().EditorCamera().gameObject.SetSelected(false, true);
            */
            MochaInterface.GameObjectVector v = new MochaInterface.GameObjectVector();
            foreach (GameObject g in _selectedGameObjects)
            {
                if (g == null)
                    continue;
                /*EditorSubsystemManaged.getInstance().QueueAction(() =>
                {
                    g.SetSelected(true, false);
                });*/
                v.Add(g);
            }
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                MochaInterface.EditorSubsystem.getInstance().SetSelectedObjects(v);
            });
        }

        [STAThread]
        public void T_EngineThread()
        {
            //MCommand.MCommands.BindCommands();
            EditorSubsystemManaged.getInstance().RegisterHooks();
            MochaCore.CoffeeConstructor.Start(true, 640, 480, false);
            while (true)
            {
                MochaCore.CoffeeConstructor.Run();
                EditorSubsystemManaged.getInstance().Tick();        //force run this after the Script Subsystem
            }
        }

        #region PROPERTYCHANGED
        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string name)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }
        #endregion
    }
}
