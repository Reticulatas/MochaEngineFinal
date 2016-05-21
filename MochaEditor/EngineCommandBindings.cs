using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Threading;
using MochaInterface;
using System.Collections.ObjectModel;

namespace MochaEditor
{
    public static class EngineCommandBindings
    {
        //griding properties (see gamecontrolpanel)
        public static int GridSize { get { return _gridSize; } set { _gridSize = value; if (EngineManagerViewModel.instance.IsConnected) { EditorSubsystem.getInstance().gridSize = value; } } }
        private static int _gridSize = 10;
        public static bool DrawGrid { get; set; }
        public static int gridExtent = 30;          //number of grid lines

        public static void CMD_SaveMap()
        {
            if (EditorSubsystem.getInstance().IsTesting())
            {
                Logger.Log("Cannot save in testing mode.");
                return;
            }
            //EngineManagerViewModel.instance.BusyMessage = "... Saving Map ...";
            EngineManagerViewModel.instance.SelectedGameObjects.Clear();     //safety check, remove the selected object so no weird binding happens
            string currentLoadedMap = EngineManagerViewModel.instance.currentLoadedMap;
            if (currentLoadedMap != null)
                EditorSubsystemManaged.getInstance().QueueAction(() => { StateSerializer ss = new StateSerializer(); ss.SaveCurrentState(currentLoadedMap); });
            else
                System.Windows.MessageBox.Show("Save As, Before Saving...");
            // EngineManagerViewModel.instance.BusyMessage = null;        }
        }

        public static void CMD_SaveMapAs()
        {
            if (EditorSubsystem.getInstance().IsTesting())
            {
                Logger.Log("Cannot save in testing mode.");
                return;
            }
            System.Windows.Application.Current.Dispatcher.Invoke(() =>
            {
                SaveFileDialog dlg = new SaveFileDialog();
                dlg.OverwritePrompt = false;
                dlg.DefaultExt = ".mocha";
                dlg.Filter = "Mocha Map Files (.mocha)|*.mocha";
                DialogResult result = dlg.ShowDialog();
                if (result == DialogResult.OK)
                {
                    // EngineManagerViewModel.instance.BusyMessage = "... Saving Map ...";
                    string filename = dlg.FileName;
                    Dispatcher.CurrentDispatcher.Invoke(() => { EngineManagerViewModel.instance.SelectedGameObjects.Clear(); });
                    EditorSubsystemManaged.getInstance().QueueAction(() =>
                    {
                        StateSerializer ss = new StateSerializer(); ss.SaveCurrentState(filename);
                    });
                    Dispatcher.CurrentDispatcher.Invoke(() => { EngineManagerViewModel.instance.currentLoadedMap = filename; });
                    // EngineManagerViewModel.instance.BusyMessage = null;
                }
            });
        }

        public static void CMD_LoadMap()
        {
            if (EditorSubsystem.getInstance().IsTesting())
            {
                Logger.Log("Cannot load in testing mode.");
                return;
            }
            System.Windows.Application.Current.Dispatcher.Invoke(() =>
          {
              OpenFileDialog dlg = new OpenFileDialog();
              dlg.DefaultExt = ".mocha";
              dlg.Filter = "Mocha Map Files (.mocha)|*.mocha";
              DialogResult result = dlg.ShowDialog();
              if (result == DialogResult.OK)
              {
                  // EngineManagerViewModel.instance.BusyMessage = "... Loading " + System.IO.Path.GetFileName(dlg.FileName) + " ...";
                  string filename = dlg.FileName;
                  EngineManagerViewModel.instance.SelectedGameObjects.Clear();
                  EditorSubsystemManaged.getInstance().QueueAction(() =>
                  {
                      GameObjectVector v = new GameObjectVector();
                      EditorSubsystem.getInstance().SetSelectedObjects(v);
                      Map.LoadMap(filename);
                      EditorSettings.AddRecentMap(filename);
                  });
                  EngineManagerViewModel.instance.currentLoadedMap = filename;
                  // EngineManagerViewModel.instance.BusyMessage = null;

              }
          });
        }

        public static void CMD_MergeMap()
        {
            if (EditorSubsystem.getInstance().IsTesting())
            {
                Logger.Log("Cannot merge in testing mode.");
                return;
            }
            System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() =>
          {
              OpenFileDialog dlg = new OpenFileDialog();
              dlg.DefaultExt = ".mocha";
              dlg.Filter = "Mocha Map Files (.mocha)|*.mocha";
              DialogResult result = dlg.ShowDialog();
              if (result == DialogResult.OK)
              {
                  // EngineManagerViewModel.instance.BusyMessage = "... Merging " + System.IO.Path.GetFileName(dlg.FileName) + " ...";
                  string filename = dlg.FileName;

                  MergeMapWindow mmw = new MergeMapWindow(filename);
                  mmw.ShowDialog();
              }
          }));
        }

        public static void CMD_SnapToGrid()
        {
            //snap
            foreach (GameObject g in EngineManagerViewModel.instance.SelectedGameObjects) 
            {
                g.transform.SetPositionX(GridSize * (float)Math.Floor((g.transform.position.x + GridSize * 0.5f) / GridSize));
                g.transform.SetPositionY(GridSize * (float)Math.Floor((g.transform.position.y + GridSize * 0.5f) / GridSize));
                g.transform.SetPositionZ(GridSize * (float)Math.Floor((g.transform.position.z + GridSize * 0.5f) / GridSize));
            }
        }

        public static void CMD_DeleteObjects()
        {
            if (EditorSubsystem.getInstance().IsTesting())
            {
                Logger.Log("Cannot delete in testing mode.");
                return;
            }
            List<GameObject> objsToDel = EngineManagerViewModel.instance.SelectedGameObjects.ToList();
            EngineManagerViewModel.instance.SelectedGameObjects.Clear();
            foreach (GameObject g in objsToDel)
            {
                EditorSubsystemManaged.getInstance().QueueAction(() => { g.Destroy(); });
                HierarchyControlSelectionManager.HierarchyControlStateManager.RequestRemoval(g);
            }
        }

        public static void CMD_AppendNumber(GameObject obj)
        {
            IEnumerable<GameObject> objenum = null;
            if (obj.transform.GetParent() == null)
                objenum = EngineManagerViewModel.instance.CurrentGameObjects.AsEnumerable<GameObject>();
            else
            {
                Converters.CTransformListToGameObjectListConverter conv = new Converters.CTransformListToGameObjectListConverter();
                objenum = (conv.Convert(obj.transform.GetParent().children,null, null, null) as ObservableCollection<GameObject>).AsEnumerable<GameObject>();
            }
            string obj_name = obj.GetName();
            for (int i = 0; i < obj_name.Length; ++i)
            {
                if (char.IsDigit(obj_name[i]))
                {
                    obj_name = obj_name.Remove(i);
                    break;
                }
            }
            obj_name = obj_name.Trim();
            int index = objenum.Count(new Func<GameObject,bool>((GameObject g)=>{ return g.GetName().Contains(obj_name); }));
            obj.SetName(obj_name + " " + index.ToString());
        }

        public static void CMD_DuplicateObjects()
        {
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                List<GameObject> selgobj = EngineManagerViewModel.instance.SelectedGameObjects.ToList();
                List<GameObject> newObjgs = new List<GameObject>();
                foreach (GameObject g in selgobj)
                {
                    GameObject newg = GameObject.From(g.Duplicate());
                    CMD_AppendNumber(newg);
                    newObjgs.Add(newg);
                }
                System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    HierarchyControlSelectionManager.HierarchyControlStateManager.DeSelectAll();
                    foreach (GameObject g in newObjgs)
                        HierarchyControlSelectionManager.HierarchyControlStateManager.GObjAdditiveSelectRefresh(g);
                }));


            });
        }

        public static void CMD_SelectAllObjects()
        {
            System.Windows.Application.Current.Dispatcher.Invoke(() =>
            {
                EngineManagerViewModel.instance.SelectedGameObjects.Clear();
                foreach (GameObject g in EngineManagerViewModel.instance.CurrentGameObjects)
                    EngineManagerViewModel.instance.SelectedGameObjects.Add(g);
            });
        }

        public static void CMD_FindObject()
        {
            Controls.ModernFindObjectWindow mmw = new Controls.ModernFindObjectWindow();
            mmw.ShowDialog();
        }
    }
}
