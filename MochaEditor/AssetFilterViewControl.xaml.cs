using System;
using System.ComponentModel;
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
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.IO;
using System.Xml;
using System.Reflection;

namespace MochaEditor
{

    public partial class AssetFilterViewControl : UserControl, FirstFloor.ModernUI.Windows.IContent, INotifyPropertyChanged
    {
        protected void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)   
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        public ObservableCollection<AssetItem> assets { get; set; }
        public ObservableCollection<AssetItem> filteredAssets { get; set; }
        public int _rowsInGrid = 50;
        public int rowsInGrid { get { return _rowsInGrid; } set { _rowsInGrid = value; PropertyChanged(this, new PropertyChangedEventArgs("rowsInGrid")); } }

        string _assetWatchedType;
        public string AssetWatchedType
        {
            get { return _assetWatchedType; }
            set
            {
                _assetWatchedType = value;
                generateContents();
            }
        }

        void generateContents()
        {
            if (assets == null)
            {
                filteredAssets = new ObservableCollection<AssetItem>();
                assets = new ObservableCollection<AssetItem>();
                FilterView.ItemsSource = assets;
            }
            assets.Clear();

            if (AssetManagerManaged.getInstance().asset_type_to_item.ContainsKey(_assetWatchedType))
            {
                foreach (AssetItem item in AssetManagerManaged.getInstance().asset_type_to_item[_assetWatchedType])
                    assets.Add(item);
            }
            filteredAssets = assets;

            FilterControl_TextChanged(null, null);
        }
                                
        public AssetFilterViewControl()
        {
            InitializeComponent();

            AssetManagerManaged.getInstance().OnAssetsChanged += AssetFilterViewControl_OnAssetsChanged;
        }

        void AssetFilterViewControl_OnAssetsChanged(object sender, EventArgs e)
        {
            generateContents();
        }

        private void ClearFilterControl_Click(object sender, RoutedEventArgs e)
        {
            FilterControl.Clear();
        }

        private bool AssetFilter(object item)
        {
            string text = FilterControl.Text;
            if (text.Length > 0 && text[0] == '*')
                text = "";

            if (String.IsNullOrWhiteSpace(text))
                return true;
            else
                return ((item as AssetItem).filename.IndexOf(text, StringComparison.OrdinalIgnoreCase) >= 0);
        }

        private void FilterControl_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (FilterView != null && FilterView.ItemsSource != null)
                CollectionViewSource.GetDefaultView(FilterView.ItemsSource).Refresh();


            /*
            if (FilterView != null)// && FilterView.ItemsSource != null)
            {
                filteredAssets.Clear();
                foreach (AssetItem item in assets)
                {
                    if (AssetFilter(item))
                    {
                        filteredAssets.Add(item);
                    }
                }
            }*/
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (FilterView.ItemsSource != null)
            {
                CollectionView view = (CollectionView)CollectionViewSource.GetDefaultView(FilterView.ItemsSource);
                view.Filter = AssetFilter;
            }

        }

        Point startPoint;

        private void FilterView_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            startPoint = e.GetPosition(null);
        }

        private void FilterView_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            // Get the current mouse position
            Point mousePos = e.GetPosition(null);
            Vector diff = startPoint - mousePos;

            if (e.LeftButton == MouseButtonState.Pressed && (Math.Abs(diff.X) > SystemParameters.MinimumHorizontalDragDistance ||
                Math.Abs(diff.Y) > SystemParameters.MinimumVerticalDragDistance))
            {
                // Get the dragged ListViewItem
                ListView listView = sender as ListView;
                ListViewItem listViewItem =
                    WPFHelper.FindAnchestor<ListViewItem>((DependencyObject)e.OriginalSource);

                if (listViewItem == null)
                    return;

                // Find the data behind the ListViewItem
                AssetItem item = (AssetItem)listView.ItemContainerGenerator.
                    ItemFromContainer(listViewItem);

                // Initialize the drag & drop operation
                DataObject dragData = new DataObject("assetFormat", item);
                DragDrop.DoDragDrop(listViewItem, dragData, DragDropEffects.Move);
            } 
        }

        public void OnFragmentNavigation(FirstFloor.ModernUI.Windows.Navigation.FragmentNavigationEventArgs e)
        {
            if (String.IsNullOrWhiteSpace(_assetWatchedType))
            {
                _assetWatchedType = e.Fragment;
                generateContents();
            }
        }

        public void OnNavigatedFrom(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
        }

        public void OnNavigatedTo(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
        }

        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
        }

        public void OnItemDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (e.OriginalSource != null)
            {
                var item = ((FrameworkElement)e.OriginalSource).DataContext as AssetItem;
                if (item != null)
                    item.OnPreview();
            }
        }

        private static void OnAssetWatchedTypePropertyChanged(DependencyObject dependencyObject,
            DependencyPropertyChangedEventArgs e)
        {
            AssetFilterViewControl afvc = dependencyObject as AssetFilterViewControl;
            afvc.AssetWatchedType = e.NewValue as string;
            afvc.OnPropertyChanged("AssetWatchedType");
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public static readonly DependencyProperty AssetWatchedTypeProperty = DependencyProperty.Register(
            "AssetWatchedType", typeof(string), typeof(AssetFilterViewControl),
            new PropertyMetadata(string.Empty, OnAssetWatchedTypePropertyChanged));

        private void ViewTypesComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (FilterView != null)
            {
                if ((ViewTypesComboBox.SelectedItem as ComboBoxItem).Content as string == "Details")
                    FilterView.ItemsPanel = this.FindResource("DetailsPanelTemplate") as ItemsPanelTemplate;
                if ((ViewTypesComboBox.SelectedItem as ComboBoxItem).Content as string == "Grid")
                    FilterView.ItemsPanel = this.FindResource("GridPanelTemplate") as ItemsPanelTemplate;
            }
        }

    }
}
