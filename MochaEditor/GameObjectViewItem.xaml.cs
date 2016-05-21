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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for GameObjectViewItem.xaml
    /// </summary>
    public partial class GameObjectViewItem : UserControl
    {
        static List<GameObjectViewItem> items = new List<GameObjectViewItem>();

        public GameObjectViewItem()
        {
            InitializeComponent();

            //default
            VisualStateManager.GoToState(this as FrameworkElement, Collapsed.Name, false);

            items.Add(this);
        }

        ~GameObjectViewItem()
        {
            items.Remove(this);
        }

        private void StackPanel_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (Expanding.CurrentState == Expanded)
                VisualStateManager.GoToState(this as FrameworkElement, Collapsed.Name, true);
            else if (Expanding.CurrentState == Collapsed)
            {
                //collapse all others
                foreach (GameObjectViewItem i in items)
                {
                    VisualStateManager.GoToState(i as FrameworkElement, Collapsed.Name, true);
                }
                VisualStateManager.GoToState(this as FrameworkElement, Expanded.Name, true);
            }
        }

        private void Delete_Click(object sender, RoutedEventArgs e)
        {
            EngineCommandBindings.CMD_DeleteObjects();
        }

        private void DuplicateButton_Click(object sender, RoutedEventArgs e)
        {
            EngineCommandBindings.CMD_DuplicateObjects();
        }

        private void Detach(GameObject g)
        {
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                g.transform.SetParent(null);

                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    GameObjectViewControl.instance.generate_hierarchytree();
                }));
            });
        }

        private void NoParentButton_Click(object sender, RoutedEventArgs e)
        {
            Detach((this.DataContext as GameObject));
            foreach (GameObject g in Models.EngineManagerModel.instance._selectedGameObjects)
            {
                if (g != this.DataContext as GameObject)
                    Detach(g);
            }
        }

        private void MakePrefabButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (GameObject g in Models.EngineManagerModel.instance._selectedGameObjects)
            {
                MochaInterface.Prefab.MakePrefabFrom(g);
            }
        }
    }
}
