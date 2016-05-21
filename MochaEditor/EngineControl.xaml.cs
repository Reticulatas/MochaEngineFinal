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
using System.Threading;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for EngineControl.xaml
    /// </summary>
    public partial class EngineControl : UserControl
    {
        public event EventHandler OnAppOpenedSuccessfully;

        public EngineControl()
        {
            InitializeComponent();

            if (Models.EngineManagerModel.instance != null)
                Models.EngineManagerModel.instance.EngineEmbedControl = this;
        } 

        public AppControl GetAppControl()
        {
            return (EngineFrame.Content as AppControl);
        }

        bool loaded = false;
        private void EngineFrame_Loaded(object sender, RoutedEventArgs e)
        {
            if (loaded) return;
            loaded = true;

            (EngineFrame.Content as AppControl).ViewModel.ExeName = "MochaEngine.exe";
            (EngineFrame.Content as AppControl).ViewModel.ConnectExisting = true;
            (EngineFrame.Content as AppControl).ViewModel.OnAppOpenedSuccessfully += new EventHandler(Route_OnAppOpenedSuccessfully);

            Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                (DataContext as EngineManagerViewModel).Connect();
            }));
        }

        void Route_OnAppOpenedSuccessfully(object sender, EventArgs e)
        {
            if (OnAppOpenedSuccessfully != null)
                OnAppOpenedSuccessfully(sender, e);
        }
        
        public void SetVisible(bool v)
        {
            if (v == true)
            {
                (EngineFrame.Content as AppControl).ViewModel.UnEmbed();
                Console.WriteLine("Unembedded");
            }
            else
            {
                (EngineFrame.Content as AppControl).ViewModel.Embed((EngineFrame.Content as AppControl));
                Console.WriteLine("embedded");
            }
        }

        void EngineControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetVisible((bool)e.NewValue);
        }

        private void ConnectEngineButton_Click(object sender, RoutedEventArgs e)
        {
            //redirect this up to the EngineManager
            (DataContext as EngineManagerViewModel).Connect();
        }

        public void Recreate()
        {
            (EngineFrame.Content as AppControl).ViewModel.Recreate((EngineFrame.Content));
        }
    }
}
