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
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MochaEditor;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace MochaEditorControls
{
    public class HierarchyDragController : INotifyPropertyChanged
    {
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetCursorPos(ref Win32Point pt);

        [StructLayout(LayoutKind.Sequential)]
        internal struct Win32Point
        {
            public Int32 X;
            public Int32 Y;
        };
        bool _isDragging = false;
        public bool IsDragging {
            get { return _isDragging; }
            set { _isDragging = value; OnPropertyChanged("IsDragging"); } 
        }
        public Window _dragdropWindow = null;
        public FrameworkElement _leftoverItem = null;

        public HierarchyDragController()
        {
        }

        public enum DropType
        {
            NORMAL,
            SEPARATOR,
            FOLDER
        }

        public void Drop(object sender, DragEventArgs e, DropType type)
        {
            //var source = WPFHelper.FindParent<HierarchyControl>(e.Data.GetData() as TextBlock);
            var source = e.Data.GetData("control") as HierarchyControl;
            var target = WPFHelper.FindParent<HierarchyControl>(sender as DependencyObject);
            var source_parent = WPFHelper.FindParent<HierarchyControl>(source);
            
            source.Effect = null;
            source.RenderTransform = null;

            //make sure we aren't double dropping
            if (source != target)
            {
                //drop into the target hierarchy control as child
                switch (type)
                {
                    case DropType.NORMAL:
                        foreach (GameObject g in EngineManagerViewModel.instance.SelectedGameObjects)
                        {
                            if (g != source.GetDataContext())
                                target.Virtual_AddChild(g);
                        }
                        target.Virtual_AddChild(source);
                        break;
                    case DropType.SEPARATOR:
                        target.Virtual_DetachChild(source);
                        break;
                    case DropType.FOLDER:
                        target.Virtual_DetachChild(source);
                        break;
                }

                //remove it from the intermediate visualization
                foreach (GameObject g in EngineManagerViewModel.instance.SelectedGameObjects)
                    source_parent.IntermediateSource.Remove(g);
                source_parent.IntermediateSource.Remove(source.myGameObject);
            }

            DestroyDragDropWindow();

            e.Handled = true;
        }

        public void DestroyDragDropWindow()
        {
            if (_dragdropWindow != null)
            {
                _dragdropWindow.Close();
                _dragdropWindow = null;
                IsDragging = false;
            }
            if (_leftoverItem != null)
            {
                _leftoverItem.Effect = null;
                _leftoverItem.RenderTransform = null;
                _leftoverItem = null;
            }
        }
        public void Drag_GiveFeedback(object sender, GiveFeedbackEventArgs e)
        {
            Win32Point w32Mouse = new Win32Point();
            GetCursorPos(ref w32Mouse);

            _dragdropWindow.Left = w32Mouse.X;
            _dragdropWindow.Top = w32Mouse.Y;

            IsDragging = true;
        }

        public void CreateDragDropWindow(FrameworkElement dragElement)
        {
            _dragdropWindow = new Window();
            _dragdropWindow.WindowStyle = WindowStyle.None;
            _dragdropWindow.AllowsTransparency = true;
            _dragdropWindow.AllowDrop = false;
            _dragdropWindow.Background = null;
            _dragdropWindow.IsHitTestVisible = false;
            _dragdropWindow.SizeToContent = SizeToContent.WidthAndHeight;
            _dragdropWindow.Topmost = true;
            _dragdropWindow.ShowInTaskbar = false;

            Rectangle r = new Rectangle();
            r.Width = ((FrameworkElement)dragElement).ActualWidth;
            r.Height = ((FrameworkElement)dragElement).ActualHeight;
            r.Fill = new VisualBrush(dragElement);
            _dragdropWindow.Content = r;

            Win32Point w32Mouse = new Win32Point();
            GetCursorPos(ref w32Mouse);

            _dragdropWindow.Left = w32Mouse.X;
            _dragdropWindow.Top = w32Mouse.Y;
            _dragdropWindow.Show();

            //show visual change on left over
            dragElement.Effect = new DropShadowEffect
            {
                Color = Colors.Black,
                Direction = 320,
                ShadowDepth = 0,
                Opacity = 0.75,
            };
            dragElement.RenderTransform = new RotateTransform(2.0f);

            _leftoverItem = dragElement;
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

    public class HierarchyControl : ItemsControl, INotifyPropertyChanged
    {
        #region DEPENDENCYPROPERTIES
        //intermediate items source property
        MochaEditor.Containers.SuppressableObservableCollection<GameObject> _intermediate_source = new MochaEditor.Containers.SuppressableObservableCollection<GameObject>();
        public MochaEditor.Containers.SuppressableObservableCollection<GameObject> IntermediateSource
        {
            get { return _intermediate_source; }
            set { _intermediate_source = value; OnPropertyChanged("IntermediateSource"); }
        }
        //front end children
        public ObservableCollection<GameObject> Children
        {
            get { return (ObservableCollection<GameObject>)this.GetValue(ChildrenProperty); }
            set { this.SetValue(ChildrenProperty, value); OnPropertyChanged("Children"); OnPropertyChanged("HasChildren"); }
        }
        public bool HasChildren
        {
            get { return (Children.Count != 0);  }
        }
        public static readonly DependencyProperty ChildrenProperty = DependencyProperty.Register(
        "Children", typeof(ObservableCollection<GameObject>), typeof(HierarchyControl), new PropertyMetadata(new ObservableCollection<GameObject>(), OnChildrenChanged, OnChildrenCoerce));

        private static object OnChildrenCoerce(DependencyObject d, object baseValue)
        {
            //(d as HierarchyControl).SetValue(HierarchyControl.ItemsSourceProperty, baseValue);
            var hc = d as HierarchyControl;
            hc.IntermediateSource.ReplaceRange(baseValue as ObservableCollection<GameObject>);
            foreach (var child_item in WPFHelper.FindVisualChildren<HierarchyControl>(hc))
            {
                child_item.IntermediateSource_CollectionChanged(null, null);
            }
            return baseValue;
        }

        private static void OnChildrenChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            OnChildrenCoerce(d, e.NewValue);
        }

        //my logical parent
        public HierarchyControl ParentHierarchyControl
        {
            get { return (HierarchyControl)this.GetValue(ParentHierarchyProperty); }
            set { this.SetValue(ParentHierarchyProperty, value); OnPropertyChanged("ParentHierarchyControl"); }
        }
        public static readonly DependencyProperty ParentHierarchyProperty = DependencyProperty.Register(
        "ParentHierarchyControl", typeof(HierarchyControl), typeof(HierarchyControl), new PropertyMetadata(null));
        //items selected at this and lower levels
        public ObservableCollection<object> RecursiveSelectedItems
        {
            get { return (ObservableCollection<object>)this.GetValue(RecursiveSelectedItemsProperty); }
            set { this.SetValue(RecursiveSelectedItemsProperty, value); OnPropertyChanged("RecursiveSelectedItems"); }
        }
        public static readonly DependencyProperty RecursiveSelectedItemsProperty = DependencyProperty.Register(
        "RecursiveSelectedItems", typeof(ObservableCollection<object>), typeof(HierarchyControl), new PropertyMetadata(null));
        public HierarchyDragController DragController
        {
            get { return (HierarchyDragController)this.GetValue(DragControllerProperty); }
            set { this.SetValue(DragControllerProperty, value); OnPropertyChanged("DragController"); }
        }
        public static readonly DependencyProperty DragControllerProperty = DependencyProperty.Register(
        "DragController", typeof(HierarchyDragController), typeof(HierarchyControl), new PropertyMetadata(new HierarchyDragController()));
        public bool IsFolder
        {
            get { return (bool)this.GetValue(IsFolderProperty); }
            set { this.SetValue(IsFolderProperty, value); OnPropertyChanged("IsFolder"); }
        }
        public static readonly DependencyProperty IsFolderProperty = DependencyProperty.Register(
        "IsFolder", typeof(bool), typeof(HierarchyControl), new PropertyMetadata(false));
        public bool IsSelected
        {
            get { return (bool)this.GetValue(IsSelectedProperty); }
            set { this.SetValue(IsSelectedProperty, value); OnPropertyChanged("IsSelected"); }
        }
        public static readonly DependencyProperty IsSelectedProperty = DependencyProperty.Register(
        "IsSelected", typeof(bool), typeof(HierarchyControl), new PropertyMetadata(false, null, IsSelectedCoerceCallback));

        public GameObject myGameObject;
        SolidColorBrush _panelColor = new SolidColorBrush(Colors.Gray);
        public SolidColorBrush PanelColor
        {
            get { return _panelColor;  }
            set { _panelColor = value; OnPropertyChanged("PanelColor"); }
        }
        #endregion

        private static object IsSelectedCoerceCallback(DependencyObject d, object baseValue)
        {
            HierarchyControl hc = d as HierarchyControl;
            return baseValue;
        }

        public HierarchyControl()
        {
            RecursiveSelectedItems = new ObservableCollection<object>();

            //set items panel
            FrameworkElementFactory factoryPanel = new FrameworkElementFactory(typeof(StackPanel));
            factoryPanel.SetValue(StackPanel.IsItemsHostProperty, true);
            ItemsPanelTemplate template = new ItemsPanelTemplate();
            template.VisualTree = factoryPanel;
            ItemsPanel = template;

            //bind parent
            Binding parentBind = new Binding();
            parentBind.RelativeSource = new RelativeSource(RelativeSourceMode.FindAncestor, typeof(HierarchyControl), 1);
            this.SetBinding(ParentHierarchyProperty, parentBind);

            FrameworkElementFactory factoryItem = new FrameworkElementFactory(typeof(HierarchyControl));
            var item_template = new DataTemplate();
            item_template.VisualTree = factoryItem;
            ItemTemplate = item_template;

            //bind intermediate children to items source 
            //this.SetBinding(ItemsSourceProperty, new Binding("Children"));

            //drag and drop binds
            this.Loaded += HierarchyControl_Loaded;
            this.Unloaded += HierarchyControl_Unloaded;
            this.GiveFeedback += DragController.Drag_GiveFeedback;

            Binding bind_int = new Binding("IntermediateSource");
            bind_int.RelativeSource = new RelativeSource(RelativeSourceMode.Self);
            this.SetBinding(HierarchyControl.ItemsSourceProperty, bind_int);
            IntermediateSource.CollectionChanged += IntermediateSource_CollectionChanged;
        }

        void HierarchyControl_Unloaded(object sender, RoutedEventArgs e)
        {
            HierarchyControlSelectionManager.HierarchyControlStateManager.DeRegisterHierarchyControl(this);
        }

        public void IntermediateSource_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.SetValue(HierarchyControl.ItemsSourceProperty, IntermediateSource);
            //recursively set children
            foreach (var child_item in WPFHelper.FindVisualChildren<HierarchyControl>(this))
            {
                child_item.IntermediateSource_CollectionChanged(this, null);
            }
        }

        bool _hasLoaded = false;
        void HierarchyControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (!_hasLoaded)
            {
                myGameObject = this.DataContext as GameObject;

                var textb_ele = this.Template.FindName("HierarchyText", this);
                var textb = textb_ele as TextBlock;
                (textb).Drop += DropRedirect;
                (textb).PreviewMouseLeftButtonUp += HierarchyControl_PreviewMouseLeftButtonUp;
                (textb).PreviewMouseRightButtonDown += HierarchyControl_PreviewMouseRightButtonDown;

                var sep_ele = this.Template.FindName("HierarchyBorder", this);
                var sepb = sep_ele as Border;
                (sepb).AllowDrop = true;
                (sepb).Drop += DropRedirectSeparator;
                (sepb).DragOver += HierarchyControl_DragOver;
                (sepb).PreviewDragLeave += HierarchyControl_DragLeave;

                //set item template and source
                if (this.GetBindingExpression(HierarchyControl.ChildrenProperty) == null)
                {
                    Binding children_bind = new Binding("transform.children");
                    children_bind.Converter = new MochaEditor.Converters.CTransformListToGameObjectListConverter();
                    this.SetBinding(HierarchyControl.ChildrenProperty, children_bind);

                    this.GetBindingExpression(HierarchyControl.ChildrenProperty).UpdateTarget();

                    this.SetValue(HierarchyControl.ItemsSourceProperty, Children);
                }

                RecomputePanelColor();

                _hasLoaded = true;
            }

            HierarchyControlSelectionManager.HierarchyControlStateManager.RegisterHierarchyControl(this);   //always do this
        }

        void RecomputePanelColor()
        {
            int depth = 0;
            HierarchyControl iter = this;
            while (iter != null)
            {
                iter = iter.ParentHierarchyControl;
                depth++;
            }
            byte c = (byte)Math.Abs(0 + (float)depth * 10.0f);
            PanelColor = new SolidColorBrush(Color.FromArgb(c, (byte)0, (byte)0, (byte)0));
        }

        void HierarchyControl_DragLeave(object sender, DragEventArgs e)
        {
            //RecomputePanelColor();
        }

        void HierarchyControl_DragOver(object sender, DragEventArgs e)
        {
            //PanelColor = new SolidColorBrush(Colors.Green);
        }

        void HierarchyControl_PreviewMouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (sender is TextBlock)
            {
                var draggedItem = WPFHelper.FindParent<HierarchyControl>(sender as TextBlock);

                //create drag and drop item
                DragController.CreateDragDropWindow(draggedItem);

                //get rid of this hierarchy control manually
                HierarchyControl_Unloaded(null, null);

                DataObject dragData = new DataObject("control", draggedItem);
                DragDrop.DoDragDrop(draggedItem, dragData, DragDropEffects.All);

                DragController.DestroyDragDropWindow();

                e.Handled = true;
            };
        }

        public void Select()
        {
            var gameObject = DataContext as GameObject;

            //select visual
            if (Keyboard.IsKeyDown(Key.LeftCtrl))
                HierarchyControlSelectionManager.RequestAdditiveSelectDeselect(this, gameObject);
            else if (Keyboard.IsKeyDown(Key.LeftShift))
                HierarchyControlSelectionManager.RequestRangeSelect(this, gameObject);
            else
                HierarchyControlSelectionManager.RequestSelect(this, gameObject);
        }

        void HierarchyControl_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            //not working ------------------
            if (sender is TextBlock)
            {
                var clickedItem = WPFHelper.FindParent<HierarchyControl>(sender as TextBlock);

                if (!clickedItem.IsFolder)
                {
                    clickedItem.Select();
                }
                else
                {
                    foreach (var child in clickedItem.Children)
                    {
                        child.IsSelected = true;
                        HierarchyControlSelectionManager.HierarchyControlStateManager.GObjAdditiveSelectRefresh(child);
                    }
                }
            }
        }

        public void DropRedirect(object sender, DragEventArgs e)
        {
            if (!IsFolder)
                DragController.Drop(sender, e, HierarchyDragController.DropType.NORMAL);
            else
                DragController.Drop(sender, e, HierarchyDragController.DropType.FOLDER);
        }
        public void DropRedirectSeparator(object sender, DragEventArgs e)
        {
            DragController.Drop(sender, e, HierarchyDragController.DropType.SEPARATOR);
        }

        static HierarchyControl()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(HierarchyControl), new FrameworkPropertyMetadata(typeof(HierarchyControl)));
        }

        public void BubbleNewSelection(object newitem)
        {
            if (ParentHierarchyControl != null)
                BubbleNewSelection(ParentHierarchyControl);
            RecursiveSelectedItems.Add(this.DataContext);
        }

        private void ItemSelected(object sender, MouseButtonEventArgs e)
        {
            BubbleNewSelection(this.DataContext);
        }

#region EXTENSION
        public GameObject GetDataContext()
        {
            return GameObject.From(DataContext as MochaInterface.GameObject);
        }

        //Add to object and to children visually
        public void Virtual_AddChild(GameObject child_obj)
        {
            if (GetDataContext() == null)
                return;
            if (Children == null)
                Children = new ObservableCollection<GameObject>();
            IntermediateSource.Add(child_obj);
            MochaInterface.CTransform this_trans = GetDataContext().transform;
            EditorSubsystemManaged.getInstance().QueueAction(new Action(() =>
            {
                child_obj.transform.SetParent(this_trans);
            }));
        }

        //Add to object and to children visually
        public void Virtual_AddChild(HierarchyControl child)
        {
            if (child.GetDataContext() == null)
                return;
            GameObject child_obj = child.GetDataContext();
            Virtual_AddChild(child_obj);
        }

        //Detach the object visually
        public void Virtual_DetachChild(HierarchyControl child)
        {
            if (child.GetDataContext() == null)
                return;
            if (Children == null)
                Children = new ObservableCollection<GameObject>();
            GameObject child_obj = child.GetDataContext();
            IntermediateSource.Add(child_obj);
            EditorSubsystemManaged.getInstance().QueueAction(new Action(() =>
            {
                child_obj.transform.SetParent(null);
            }));
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
    }
}
