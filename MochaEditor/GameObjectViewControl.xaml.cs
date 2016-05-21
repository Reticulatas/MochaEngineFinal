using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
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
using System.Windows.Threading;
using System.Threading;
using MochaInterface;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for GameObjectViewControl.xaml
    /// </summary>

    public class GameObjTreeViewItem : TreeViewItem, IComparable
    {
        public GameObject gameObject { set; get; }
        string gameObjectName;
        public bool selected;
        public GameObjTreeViewItem(GameObject g)
        {
            gameObject = g;
            gameObjectName = g.GetName();

            Header = new GameObjectViewItem();
            (Header as GameObjectViewItem).DataContext = g;

            Dispatcher.CurrentDispatcher.BeginInvoke(new Action(() =>
            {
                this.Selected += GameObjTreeViewItem_Selected;
                this.Unselected += GameObjTreeViewItem_Unselected;
            }));
            EngineManagerViewModel.instance.SelectedGameObjects.CollectionChanged += SelectedGameObjects_CollectionChanged;
        }

        void SelectedGameObjects_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (EngineManagerViewModel.instance.SelectedGameObjects.Contains(gameObject))
                SetSelected(true, (EngineManagerViewModel.instance.SelectedGameObjects.Count == 1), false);
            else
                SetSelected(false, (EngineManagerViewModel.instance.SelectedGameObjects.Count == 1), false);

            //this.InvalidateVisual();
        }

        ~GameObjTreeViewItem()
        {
            //EngineManagerViewModel.instance.SelectedGameObjects.CollectionChanged
        }

        void GameObjTreeViewItem_Unselected(object sender, RoutedEventArgs e)
        {
            //SetSelected(false, false);
            e.Handled = true;
        }

        void GameObjTreeViewItem_Selected(object sender, RoutedEventArgs e)
        {
            SetSelected(true, (Input.GetHeld(0, "AdditiveMod") != 1.0f));
            e.Handled = true;
        }

        public void SetSelected(bool v, bool unselectOthers = true, bool dispatchChange = true)
        {
            selected = v;
            if (selected)
            {
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    (Header as GameObjectViewItem).Background = new SolidColorBrush(System.Windows.Media.Colors.DarkGreen);
                }));
                this.BringIntoView();
            }
            else
            {
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    (Header as GameObjectViewItem).Background = new SolidColorBrush(System.Windows.Media.Colors.Transparent);
                    //this.Background = new SolidColorBrush(System.Windows.Media.Colors.Transparent);
                }));
            }
            
            if (dispatchChange)
            {
                if (selected)
                {
                    if (unselectOthers)
                        EngineManagerViewModel.instance.SelectedGameObjects.Clear();
                    EngineManagerViewModel.instance.SelectedGameObjects.Add(gameObject);
                    EngineManagerViewModel.instance.ForceSelectedGameObjectRefresh();
                }
                else
                {
                    EngineManagerViewModel.instance.SelectedGameObjects.Remove(gameObject);
                    if (unselectOthers)
                        EngineManagerViewModel.instance.SelectedGameObjects.Clear();
                    EngineManagerViewModel.instance.ForceSelectedGameObjectRefresh();
                }
            }
        }

        public override string ToString()
        {
            return gameObjectName;
        }

        public int CompareTo(object obj)
        {
            if (obj == null)
                return -1;
            GameObjTreeViewItem otherItem = obj as GameObjTreeViewItem;
            if (otherItem == null)
                return -1;
            return gameObjectName.CompareTo(otherItem.gameObjectName);
        }
    }

    public partial class GameObjectViewControl : UserControl, INotifyPropertyChanged
    {
        object newObject_button_orig_content;
        static GameObjectViewControl _instance;
        public static GameObjectViewControl instance { get { return _instance; } }

        public GameObjectViewControl()
        {
            InitializeComponent();
            _instance = this;
        }

        private void NewObject_Button_Click(object sender, RoutedEventArgs e)
        {
            if (newObject_button_orig_content == null)
                newObject_button_orig_content = NewObject_Button.Content;
            TextBox box = new TextBox();
            box.ToolTip = "New object name";
            box.KeyDown += box_KeyDown;
            box.LostKeyboardFocus += box_LostKeyboardFocus;
            box.LostFocus += box_LostFocus;

            NewObject_Button.Content = box;
            box.MinHeight = 30;
            box.MinWidth = 150;
            box.Background = new SolidColorBrush(System.Windows.Media.Colors.DarkSlateGray);

            Dispatcher.BeginInvoke(DispatcherPriority.Input, new Action(delegate()
            {
                box.Focus();         // Set Logical Focus
                Keyboard.Focus(box); // Set Keyboard Focus
            }));
        }

        void box_LostFocus(object sender, RoutedEventArgs e)
        {
            NewObject_Button.Content = newObject_button_orig_content;
        }

        void box_LostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            NewObject_Button.Content = newObject_button_orig_content;
        }

        void box_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                string name = (sender as TextBox).Text;
                if (String.IsNullOrWhiteSpace(name))
                    return;
                EditorSubsystemManaged.getInstance().QueueAction(() =>
                {
                    GameObject g = GameObject.From(ObjectFactory.getInstance().MakeNewGameObject(name, GSM.getInstance().getActiveState()));
                    g.transform.SetPosition(EditorSubsystem.getInstance().EditorCamera().gameObject.transform.GetGlobalPosition().Add(
                        EditorSubsystem.getInstance().EditorCamera().gameObject.transform.GetForwardVector().Mul(3.0f)));
                    Dispatcher.Invoke(() =>
                    {
                        NewObject_Button.Content = newObject_button_orig_content;
                        HierarchyControlSelectionManager.HierarchyControlStateManager.GObjSelectRefresh(g);
                    });
                });
            }
        }

        void SelectedGameObjects_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            /*
            if (e.OldItems != null)
            {
                foreach (GameObject g in e.OldItems)
                    if (cachedItems.ContainsKey(g))
                        cachedItems[g].SetSelected(false, false, false);
            }
            if (e.NewItems != null)
            {
                foreach (GameObject g in e.NewItems)
                    if (cachedItems.ContainsKey(g))
                        cachedItems[g].SetSelected(true, false, false);
            }*/
        }

        //ObservableCollection<GameObjTreeViewItem> _hierarchyItems = new ObservableCollection<GameObjTreeViewItem>();
        ObservableCollection<GameObject> _hierarchyItems = new ObservableCollection<GameObject>();
        //public ObservableCollection<GameObjTreeViewItem> hierarchyItems { get { return _hierarchyItems; } set { _hierarchyItems = value; OnPropertyChanged("hierarchyItems");  } }
        public ObservableCollection<GameObject> hierarchyItems { get { return _hierarchyItems; } set { _hierarchyItems = value; OnPropertyChanged("hierarchyItems");  } }
        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            EngineManagerViewModel.instance.CurrentGameObjects.CollectionChanged += _currentGameObjects_CollectionChanged;
            EngineManagerViewModel.instance.SelectedGameObjects.CollectionChanged += SelectedGameObjects_CollectionChanged;

        }

        //Dictionary<GameObject, GameObjTreeViewItem> cachedItems = new Dictionary<GameObject, GameObjTreeViewItem>();
        /*
        void generate_hierarchytree_addgobj(GameObject g, GameObject parent, ObservableCollection<GameObject> rootItems)
        {
            var gobjtree = new GameObjTreeViewItem(g);
            //cachedItems.Add(g, gobjtree);

            if (parent == null)
            {
                rootItems.Add(gobjtree);
            }
            else
            {
                parent.Items.Add(gobjtree);
            }

            int children_count = g.transform.children.Count;
            for (int i = 0; i < children_count; ++i)
            {
                generate_hierarchytree_addgobj(g.transform.children.ElementAt(i).gameObject, gobjtree, null);    //don't even bother to pass the root down
            }
            gobjtree.Items.BubbleSort();
        }
        */

        public void generate_hierarchytree()
        {
            GameObject selectedObject = null;
            if (EngineManagerViewModel.instance.SelectedGameObjects.Count != 0)
                selectedObject = EngineManagerViewModel.instance.SelectedGameObjects[0];
            ObservableCollection<GameObject> rootItems = new ObservableCollection<GameObject>();
            //cachedItems.Clear();

            //regenerate
            foreach (GameObject g in Models.EngineManagerModel.instance._currentGameObjects)
            {
                //skip objects with parents, the recursion will pick them up
                if (g.transform.GetParent() == null)
                {
                    rootItems.Add(g);
                    //generate_hierarchytree_addgobj(g, null, rootItems);
                }
            }

            rootItems.BubbleSort();

            hierarchyItems = rootItems;

            //try to maintain the prev selected item
            /*var gobjquery = from g in hierarchyItems where g.gameObject == selectedObject select g;
            foreach (var gobjitem in gobjquery)
            {
                gobjitem.IsSelected = true; // SetSelected(true, false);
            }*/
        }

        void _currentGameObjects_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            generate_hierarchytree();
        }

        /*

        #region TREEVIEWDRAGDROP
        Point _lastMouseDown;
        TreeViewItem draggedItem, _target;
        private bool CheckDropTarget(TreeViewItem _sourceItem, TreeViewItem _targetItem)
        {
            //Check whether the target item is meeting your condition
            bool _isEqual = false;
            GameObjTreeViewItem _sourceGItem = _sourceItem as GameObjTreeViewItem;
            GameObjTreeViewItem _targetGItem = _targetItem as GameObjTreeViewItem;
            if (_sourceGItem.gameObject != _targetGItem.gameObject)
            {
                _isEqual = true;
            }
            return _isEqual;

        }

        private bool IsRecursiveChildOf(TreeViewItem _newItem, TreeViewItem _targetItem)
        {
            return IsRecursiveChildOf_r(_newItem, _targetItem);
        }
        private bool IsRecursiveChildOf_r(TreeViewItem _newItem, TreeViewItem _targetItem)
        {
            foreach (TreeViewItem i in _targetItem.Items)
            {
                if (i == _newItem)
                    return true;
                return IsRecursiveChildOf_r(_newItem, i);
            }
            return false;
        }

        private void CopyItem(TreeViewItem _sourceItem, TreeViewItem _targetItem)
        {
            //make sure it's not recursive
            if (IsRecursiveChildOf(_sourceItem, _targetItem))
                return;

                //adding dragged TreeViewItem in target TreeViewItem
                addChild(_sourceItem, _targetItem);

                var parentItem = _targetItem as GameObjTreeViewItem;
                var childItem = _sourceItem as GameObjTreeViewItem;
                childItem.gameObject.transform.SetParent(parentItem.gameObject.transform);

                //finding Parent TreeViewItem of dragged TreeViewItem 
                TreeViewItem ParentItem = FindVisualParent<TreeViewItem>(_sourceItem);
                // if parent is null then remove from TreeView else remove from Parent TreeViewItem
                if (ParentItem == null)
                {
                    hierarchyItems.Remove(_sourceItem as GameObjTreeViewItem);
                }
                else
                {
                    ParentItem.Items.Remove(_sourceItem);
                }
        }
        public void addChild(TreeViewItem _sourceItem, TreeViewItem _targetItem)
        {
            // add item in target TreeViewItem 
            var _sourceGItem = _sourceItem as GameObjTreeViewItem;
            var _targetGItem = _targetItem as GameObjTreeViewItem;
            GameObjTreeViewItem item1 = new GameObjTreeViewItem(_sourceGItem.gameObject);
            _targetItem.Items.Add(item1);
            foreach (TreeViewItem item in _sourceItem.Items)
            {
                addChild(item, item1);
            }
        }
        static TObject FindVisualParent<TObject>(UIElement child) where TObject : UIElement
        {
            if (child == null)
            {
                return null;
            }

            UIElement parent = VisualTreeHelper.GetParent(child) as UIElement;

            while (parent != null)
            {
                TObject found = parent as TObject;
                if (found != null)
                {
                    return found;
                }
                else
                {
                    parent = VisualTreeHelper.GetParent(parent) as UIElement;
                }
            }

            return null;
        }
        private TreeViewItem GetNearestContainer(UIElement element)
        {
            // Walk up the element tree to the nearest tree view item.
            TreeViewItem container = element as TreeViewItem;
            while ((container == null) && (element != null))
            {
                element = VisualTreeHelper.GetParent(element) as UIElement;
                container = element as TreeViewItem;
            }
            return container;
        }
        private void TreeView_MouseDown
     (object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
            {
                _lastMouseDown = e.GetPosition(HierarchyTreeView);
            }
        }
        private void treeView_MouseMove(object sender, MouseEventArgs e)
        {
                if (e.LeftButton == MouseButtonState.Pressed)
                {
                    Point currentPosition = e.GetPosition(HierarchyTreeView);

                    if ((Math.Abs(currentPosition.X - _lastMouseDown.X) > 90.0) ||
                        (Math.Abs(currentPosition.Y - _lastMouseDown.Y) > 90.0))
                    {
                        draggedItem = (TreeViewItem)HierarchyTreeView.SelectedItem;
                        if (draggedItem != null)
                        {
                            DragDropEffects finalDropEffect =
                DragDrop.DoDragDrop(HierarchyTreeView,
                    HierarchyTreeView.SelectedValue,
                                DragDropEffects.Move);
                            //Checking target is not null and item is 
                            //dragging(moving)
                            if ((finalDropEffect == DragDropEffects.Move) &&
                    (_target != null))
                            {
                                if ((draggedItem as GameObjTreeViewItem).gameObject != (_target as GameObjTreeViewItem).gameObject)
                                {
                                    // A Move drop was accepted
                                    CopyItem(draggedItem, _target);
                                    _target = null;
                                    draggedItem = null;
                                }
                            }
                        }
                    }
                }
        }

        private void treeView_DragOver(object sender, DragEventArgs e)
        {
                Point currentPosition = e.GetPosition(HierarchyTreeView);

                if ((Math.Abs(currentPosition.X - _lastMouseDown.X) > 10.0) ||
                   (Math.Abs(currentPosition.Y - _lastMouseDown.Y) > 10.0))
                {
                    // Verify that this is a valid drop and then store the drop target
                    TreeViewItem item = GetNearestContainer
                    (e.OriginalSource as UIElement);
                    if (CheckDropTarget(draggedItem, item))
                    {
                        e.Effects = DragDropEffects.Move;
                    }
                    else
                    {
                        e.Effects = DragDropEffects.None;
                    }
                }
                e.Handled = true;
        }

        private void treeView_Drop(object sender, DragEventArgs e)
        {
                e.Effects = DragDropEffects.None;
                e.Handled = true;

                // Verify that this is a valid drop and then store the drop target
                TreeViewItem TargetItem = GetNearestContainer
                    (e.OriginalSource as UIElement);
                if (TargetItem != null && draggedItem != null)
                {
                    _target = TargetItem;
                    e.Effects = DragDropEffects.Move;
                }
        }
        #endregion
        */

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
