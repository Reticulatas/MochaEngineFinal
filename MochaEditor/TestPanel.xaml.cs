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
    /// Interaction logic for TestPanel.xaml
    /// </summary>
    public partial class TestPanel : UserControl
    {
        public TestPanel()
        {
            InitializeComponent();
        }

        void SetTestButtonStates(bool testing)
        {

            if (testing) {
                StartPanelButtons.Visibility = System.Windows.Visibility.Collapsed;
                StopTestingButton.Visibility = System.Windows.Visibility.Visible;
            }
            else {
                StartPanelButtons.Visibility = System.Windows.Visibility.Visible;
                StopTestingButton.Visibility = System.Windows.Visibility.Collapsed;
            }
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if ((DataContext as EngineManagerViewModel).IsConnected)
                SetTestButtonStates(EditorSubsystem.getInstance().IsTesting());

            MochaScriptSystem.ScriptSubsystemManaged.HasReloaded += TestPanel_HasReloaded;
            //TestPanel_HasReloaded(null, null);
        }

        void TestPanel_HasReloaded(object sender, EventArgs e)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                StartPanelButtons.IsEnabled = !MochaScriptSystem.ScriptSubsystemManaged.getInstance().HasErrors;
                if (!MochaScriptSystem.ScriptSubsystemManaged.getInstance().HasErrors)
                    ErrorText.Visibility = System.Windows.Visibility.Collapsed;
                else
                    ErrorText.Visibility = System.Windows.Visibility.Visible;
            }));
        }

        private bool BeginTest()
        {
            MochaScriptSystem.ScriptSubsystemManaged.getInstance().Reload();
            if (MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetHasErrors())
                return false;

            EngineManagerViewModel.instance.SelectedGameObjects.Clear();
            EngineManagerViewModel.instance.ForceSelectedGameObjectRefresh();

            EditorSubsystem.getInstance().BeginTesting();
            Dispatcher.Invoke(() => SetTestButtonStates(true));
            return true;
        }

        private void TestStartButton_Click(object sender, RoutedEventArgs e)
        {
            if ((DataContext as EngineManagerViewModel).IsConnected)
            {
                if (EditorSubsystem.getInstance().IsTesting())
                    return;
                EditorSubsystemManaged.getInstance().QueueAction(() =>
                {
                    StateSerializer ss = new StateSerializer(); ss.SaveCurrentState("_Testing_Temp.mocha");
                });
                EditorSubsystemManaged.getInstance().QueueAction(() =>
                {
                    BeginTest();
                });
            }
        }

        private void StopTestingButton_Click(object sender, RoutedEventArgs e)
        {
            if ((DataContext as EngineManagerViewModel).IsConnected)
            {
                if (!EditorSubsystem.getInstance().IsTesting())
                    return;
                EditorSubsystemManaged.getInstance().QueueAction(() =>
                {
                    EditorSubsystem.getInstance().EndTesting();
                    Dispatcher.Invoke(() => SetTestButtonStates(false));
                });

                Input.SetMouseVisible(true);
                Input.SetMouseLock(false);
            }
        }

        private void TestCurrentPosButton_Click(object sender, RoutedEventArgs e)
        {
            if ((DataContext as EngineManagerViewModel).IsConnected)
            {
                if (EditorSubsystem.getInstance().IsTesting())
                    return;
                EditorSubsystemManaged.getInstance().QueueAction(() =>
                {
                    StateSerializer ss = new StateSerializer(); ss.SaveCurrentState("_Testing_Temp.mocha");
                });
                EditorSubsystemManaged.getInstance().QueueAction(() =>
                {
                    if (BeginTest())
                    {
                        GameObject playerObj = GameObject.From(GameObject.GetGameObjectWithTag("Player"));
                        if (playerObj == null)
                            MessageBox.Show("Test from Current Position requires\nan object with the \'Player\' tag set.");
                        else
                            playerObj.transform.SetPosition(EditorSubsystem.getInstance().EditorCamera().gameObject.transform.GetGlobalPosition());
                    }
                });
            } 
        }
    }
}
