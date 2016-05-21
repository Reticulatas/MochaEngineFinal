using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml;
using MochaInterface;
using System.IO;
using Microsoft.Xna.Framework;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for ComponentPropertiesControl.xaml
    /// </summary>

    public class LayerMaskComboBoxItem
    {
        public string layer_name { get; set; }
        public GameObject bound_object { get; set; }
        public bool _is_selected;
        public bool is_selected { get { return _is_selected; } set {
            _is_selected = value;
            if (bound_object != null)
            {
                if (_is_selected)
                    bound_object.mLayerMask.AddLayer((LayerMask.Layers)(1 << my_index));
                else
                    bound_object.mLayerMask.RemoveLayer((LayerMask.Layers)(1 << my_index));
            }
        
        } }

        public int my_index;
    }

    public class LayerMaskToComboBoxItemConv : IValueConverter
    {
        static string[] layernames = {"0","1","2","3","4","5","6","UI"};
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            ObservableCollection<LayerMaskComboBoxItem> items = new ObservableCollection<LayerMaskComboBoxItem>();
            for (int i = 0; i < 8; ++i)
            {
                LayerMaskComboBoxItem item = new LayerMaskComboBoxItem();

                item.my_index = i;
                item.layer_name = layernames[i];
                item.bound_object = EngineManagerViewModel.instance.SelectedGameObjects.ElementAt(0);
                int mask = (item.bound_object.mLayerMask.GetLayerMasks());
                item._is_selected = (mask & (1 << i)) != 0;
              

                items.Add(item);
            }
            return items;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    public class ScriptFilterFileLoader
    {
        public Dictionary<string, List<string>> scriptFilters = new Dictionary<string,List<string>>();
        public ScriptFilterFileLoader()
        {
            string rootName = "[Root]";
            scriptFilters.Clear();
            scriptFilters.Add(rootName, new List<string>());

            string scriptDir = @"Assets\Scripts";
            foreach (string file in Directory.EnumerateFiles(scriptDir, "*.cs", SearchOption.TopDirectoryOnly))
                scriptFilters[rootName].Add(file.Substring(scriptDir.Length+1));
            foreach (string dir in Directory.EnumerateDirectories(scriptDir))
            {
                string dirName = dir.Substring(scriptDir.Length + 1);
                scriptFilters.Add(dirName, new List<string>());
                foreach (string file in Directory.EnumerateFiles(dir, "*.cs", SearchOption.AllDirectories))
                    scriptFilters[dirName].Add(file.Substring(dir.Length + 1));
            }
        }
    }

    public class ComponentFilterFileLoader
    {
        public static string FormatHeader(string s)
        {
            if (s == null)
                return null;
            if (s.Count() == 0)
                return null;
            int index = 0;
            if (s[0] == 'C')
                index = 1;
            s = s.Insert(index, "_");
            return s;
        }
        public class SubFilterList
        {
            public List<object> sub_list = new List<object>();
            public string header;
        }
        public List<SubFilterList> raw_filter = new List<SubFilterList>();
        public ComponentFilterFileLoader()
        {
            List<string> modCompTypes = new List<string>(EngineManagerViewModel.instance.ComponentTypes);

            using (XmlReader reader = XmlReader.Create("Assets/Config/ComponentFilters.xml"))
            {
                while (reader.ReadToFollowing("Filter"))
                {
                    reader.MoveToAttribute("name");
                    string filter_type = reader.Value;

                    raw_filter.Add(new SubFilterList() {header = filter_type});
                    do
                    {
                        reader.Read();
                        if (reader.NodeType == XmlNodeType.Element)
                        {
                            string comp_types = reader.ReadElementContentAsString();
                            modCompTypes.Remove(comp_types);
                            raw_filter.Last().sub_list.Add(comp_types);
                        }
                    } while (reader.Name != "Filter");
                }

                raw_filter.Last().sub_list.AddRange(modCompTypes);
            }
        }
    }

    public partial class ComponentPropertiesControl : UserControl
    {
        static ScriptFilterFileLoader sffl = new ScriptFilterFileLoader();

        public ComponentPropertiesControl()
        {
            InitializeComponent();
            ComponentListMenuControl_Loaded(null, null);
        }
        private void NewTagTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (String.IsNullOrWhiteSpace(NewTagTextBox.Text))
                return;

            if (e.Key != Key.Enter)
                return;

            (NewTagTextBox.DataContext as GameObject).SetFlag(NewTagTextBox.Text);
            (TagComboBox.ItemsSource as ObservableCollection<object>).Add(NewTagTextBox.Text);

            NewTagTextBox.Text = "";
        }

        private void RemoveTagButton_Click(object sender, RoutedEventArgs e)
        {
            var item = (sender as Button).DataContext as string;
            (NewTagTextBox.DataContext as GameObject).ClearFlag(item);
            (TagComboBox.ItemsSource as ObservableCollection<object>).Remove(item);
        }

        private void PrimaryComponentTypesHeader_Loaded(object sender, RoutedEventArgs e)
        {
        }

        void ComponentTypes_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            PrimaryComponentTypesHeader.Items.Clear();

            ComponentFilterFileLoader cf = new ComponentFilterFileLoader();

            foreach (var filterlist in cf.raw_filter)
            {
                MenuItem menu_item = new MenuItem();
                menu_item.Header = ComponentFilterFileLoader.FormatHeader(filterlist.header);
                PrimaryComponentTypesHeader.Items.Add(menu_item);
                foreach (string compname in filterlist.sub_list)
                {
                    MenuItem comp_menu_item = new MenuItem();
                    comp_menu_item.Header = ComponentFilterFileLoader.FormatHeader(compname);
                    if (!EngineManagerViewModel.instance.ComponentTypes.Contains(compname))
                        comp_menu_item.IsEnabled = false;
                    comp_menu_item.Click += menu_item_Click;
                    comp_menu_item.DataContext = compname;
                    menu_item.Items.Add(comp_menu_item);
                    
                }
            }
        }

        void menu_item_Click(object sender, RoutedEventArgs e)
        {
            var menuitem = e.Source as MenuItem;
            EngineManagerViewModel view = (this.DataContext as EngineManagerViewModel);
            e.Handled = true;
            if (view.SelectedGameObjects.Count == 0)
                return;
            string compname = menuitem.DataContext as String;
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                foreach (GameObject g in view.SelectedGameObjects)
                    g.RequireComponentByName(compname);
                Dispatcher.Invoke(() =>
                {
                    ComponentListViewControl.GetBindingExpression(ListView.ItemsSourceProperty).UpdateTarget();
                });
            });
        }

        bool loaded = false;
        private void ComponentListMenuControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (!loaded)
                EngineManagerViewModel.instance.ComponentTypes.CollectionChanged += ComponentTypes_CollectionChanged;
            loaded = true;
        }

        private void LayerMaskComboBox_Checked(object sender, RoutedEventArgs e)
        {
            var item = this.DataContext as LayerMaskComboBoxItem;
            if (item == null)
                return;

            item.is_selected = (sender as CheckBox).IsChecked.Value;
        }

        private void ApplyButton_Click(object sender, RoutedEventArgs e)
        {
            GameObject g = ApplyButton.DataContext as GameObject;
            if (g == null)
                return;
            if (g.linkedPrefab == null)
                return;

            EditorSubsystemManaged.getInstance().QueueAction(new Action(() =>
            {
                g.WriteToPrefab();
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    EngineManagerViewModel.instance.ForceRefreshGameObjects();
                }));
            }));
        }

        private void DetachButton_Click(object sender, RoutedEventArgs e)
        {
            GameObject g = ApplyButton.DataContext as GameObject;
            if (g == null)
                return;
            if (g.linkedPrefab == null)
                return;

            EditorSubsystemManaged.getInstance().QueueAction(new Action(() =>
            {
                g.DetachFromPrefab();
            }));
        }

        private void RevertButton_Click(object sender, RoutedEventArgs e)
        {
            GameObject g = ApplyButton.DataContext as GameObject;
            if (g == null)
                return;
            if (g.linkedPrefab == null)
                return;

            EngineManagerViewModel.instance.ForceSetSelectedGameObject(null);

            GameObject newg = null;
            EditorSubsystemManaged.getInstance().QueueAction(new Action(() =>
            {
                newg = GameObject.From(g.RevertToPrefab());
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    EngineManagerViewModel.instance.ForceSetSelectedGameObject(newg);
                    EngineManagerViewModel.instance.ForceRefreshGameObjects();
                }));
            }));
        }

        private void MakePrefabButton_Click(object sender, RoutedEventArgs e)
        {
            StringBuilder ss = new StringBuilder();
            EditorSubsystemManaged.getInstance().QueueActionAndBlock(new Action(() =>
            {
                foreach (GameObject g in Models.EngineManagerModel.instance._selectedGameObjects)
                {
                    MochaInterface.Prefab.MakePrefabFrom(g);
                    ss.AppendLine(g.GetName());
                }
            }));

            Controls.ModernModalWindow mmw = new Controls.ModernModalWindow("Prefabs created: " + ss.ToString());
            mmw.ShowDialog();
        }

        private void ComponentListView_Drop(object sender, DragEventArgs e)
        {
            //JOSH TODO
            if (e.OriginalSource != null)
            {

                var item = e.Data.GetData("assetFormat") as AssetItem;
                //AssetHandle.
                if (item != null)
                    Console.Write(item.type);

                GameObject g = ApplyButton.DataContext as GameObject;
                CMeshRenderer cRenderer = g.GetComponent<CMeshRenderer>();
                if (cRenderer != null)
                {
                    //cRenderer.GetMaterial
                    
                    //Material mat;
                    //if (item.type == "Materials")
                    //{
                    //    Console.Write(g.Names);
                    //    mat = e.Data.GetData("assetFormat") as Material;
                    //    //cRenderer.SetMaterial(mat);
                    //}
                    
                    
                }//item.CanDropOn
            }
        }

        private void ComponentListView_DragEnter(object sender, DragEventArgs e)
        {

        }

        private void AddScriptHeader_Loaded(object sender, RoutedEventArgs e)
        {
            foreach (string dir in sffl.scriptFilters.Keys)
            {
                MenuItem diritem = new MenuItem();
                diritem.Header = dir;
                AddScriptHeader.Items.Add(diritem);
                foreach (string file in sffl.scriptFilters[dir])
                {
                    MenuItem scriptitem = new MenuItem();
                    scriptitem.Header = file;
                    diritem.Items.Add(scriptitem);
                    scriptitem.Click += scriptitem_Click;
                    scriptitem.DataContext = file;
                }
            }
        }

        void scriptitem_Click(object sender, RoutedEventArgs e)
        {
            var menuitem = e.Source as MenuItem;
            EngineManagerViewModel view = (this.DataContext as EngineManagerViewModel);
            e.Handled = true;
            if (view.SelectedGameObjects.Count == 0)
                return;
            string scriptname = menuitem.DataContext as String;
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                foreach (GameObject g in view.SelectedGameObjects)
                {
                    CScript cs = CastingHelper.CastTo<CScript>(g.RequireComponentByName("CScript"));
                    cs.scriptAsset = AssetManager.getInstance().GetAsset(scriptname);
                }
                Dispatcher.BeginInvoke(new Action(() =>
                {
                    ComponentListViewControl.GetBindingExpression(ListView.ItemsSourceProperty).UpdateTarget();
                }));
            });
        }

        private void AlignViewButton_Click(object sender, RoutedEventArgs e)
        {
            EngineManagerViewModel view = (this.DataContext as EngineManagerViewModel);
            e.Handled = true;
            if (view.SelectedGameObjects.Count == 0)
                return;
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                foreach (GameObject g in view.SelectedGameObjects)
                {
                    MochaInterface.GameObject ed_cam = EditorSubsystem.getInstance().EditorCamera().gameObject;
                    g.transform.SetPosition(ed_cam.transform.position + (Vector3)ed_cam.transform.GetForwardVector() * 5);
                }
            });
        }

        private void GroundSnapButton_Click(object sender, RoutedEventArgs e)
        {
            EngineManagerViewModel view = (this.DataContext as EngineManagerViewModel);
            e.Handled = true;
            if (view.SelectedGameObjects.Count == 0)
                return;
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                foreach (GameObject g in view.SelectedGameObjects)
                {
                    RayCastResult result = PhysicEngine.getInstance().RayCast3D_Output(g.transform.position, Vector3.Down, 1000);
                    if (result.hit)
                        g.transform.SetPosition(result.intersection);
                }
            });
        }
    }
}
