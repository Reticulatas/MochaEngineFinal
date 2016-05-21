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
using MochaInterface;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for ComponentViewControl.xaml
    /// </summary>
    public partial class ComponentViewControl : UserControl
    {
        public ComponentViewControl()
        {
            InitializeComponent();
        }

        private void RemoveCompButton_Click(object sender, RoutedEventArgs e)
        {
            Component c = (DataContext as Component);
            EditorSubsystemManaged.getInstance().QueueAction(() => { c.Destroy(); });
            EditorSubsystemManaged.getInstance().QueueWaitFrame();
            EditorSubsystemManaged.getInstance().QueueAction(() => {
                Dispatcher.Invoke(new Action(() =>
                {
                    var dc = EngineManagerViewModel.instance.SelectedGameObjects;
                    EngineManagerViewModel.instance.SelectedGameObjects = null;
                    EngineManagerViewModel.instance.SelectedGameObjects = dc;
                }));
            });
            RemoveCompButton.IsEnabled = false;
        }

        private void EnableCompButton_Click(object sender, RoutedEventArgs e)
        {
            Component c = (DataContext as Component);
            bool enabled = EnableCompButton.IsChecked.Value;
            EditorSubsystemManaged.getInstance().QueueAction(() => c.setEnabled(enabled));
        }

        private void CopyValues_Click(object sender, RoutedEventArgs e)
        {
            Component c = (DataContext as Component);
            CopyPasteManager.Copy(c);
        }

        private void PaseValues_Click(object sender, RoutedEventArgs e)
        {
            Component c = (DataContext as Component);
            CopyPasteManager.Paste(c);
        }

        private void OptionalButton_Click(object sender, RoutedEventArgs e)
        {
            (sender as RadioButton).ContextMenu.IsEnabled = true;
            (sender as RadioButton).ContextMenu.PlacementTarget = (sender as RadioButton);
            (sender as RadioButton).ContextMenu.Placement = System.Windows.Controls.Primitives.PlacementMode.Bottom;
            (sender as RadioButton).ContextMenu.IsOpen = true;
        }
    }
}
