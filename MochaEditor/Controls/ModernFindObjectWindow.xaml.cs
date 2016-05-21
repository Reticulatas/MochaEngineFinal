using System;
using System.Collections.Generic;
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
using System.Windows.Shapes;
using FirstFloor.ModernUI.Windows.Controls;
using FirstFloor.ModernUI.Presentation;
using System.ComponentModel;
using System.Collections.ObjectModel;


namespace MochaEditor.Controls
{
    /// <summary>
    /// Interaction logic for ModernFindObjectWindow.xaml
    /// </summary>
    public partial class ModernFindObjectWindow : ModernDialog
    {
        ObservableCollection<GameObject> foundObjects = null;
        public ObservableCollection<GameObject> firstfoundObjects { get; set; }

        public ModernFindObjectWindow()
        {
            firstfoundObjects = new ObservableCollection<GameObject>();
            foundObjects = new ObservableCollection<GameObject>();
            foreach (var g in EngineManagerViewModel.instance.CurrentGameObjects)
                foundObjects.Add(g);
            InitializeComponent();
        }

        private void FoundObjectsList_PreviewKeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                this.Close();
            }
        }

        private void InputText_TextChanged(object sender, TextChangedEventArgs e)
        {
            string input = InputText.Text;

            firstfoundObjects.Clear();
            if (String.IsNullOrWhiteSpace(input))
                return;

            ListExtension.BubbleSortGameObjectLevenstein(foundObjects, input);
            for (int i = 0; i < foundObjects.Count && i < 10; ++i)
                firstfoundObjects.Add(foundObjects[i]);
        }

        private void ModernDialog_Loaded(object sender, RoutedEventArgs e)
        {
            InputText.Focus();
        }

        private void ModernDialog_Closing(object sender, CancelEventArgs e)
        {
            var item = FoundObjectsList.SelectedItem;

            if (item != null)
            {
                ((GameObject)item).IsSelected = true;
                HierarchyControlSelectionManager.HierarchyControlStateManager.GObjSelectRefresh((GameObject)item);
            }
        }
    }
}
