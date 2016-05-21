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
using System.Windows.Threading;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for Workspace_Performance.xaml
    /// </summary>
    public partial class Workspace_Performance : UserControl
    {
        ViewModel.PerformanceViewModel view = null;
        DispatcherTimer refreshTimer = null;

        public Workspace_Performance()
        {
            InitializeComponent();

            view = new ViewModel.PerformanceViewModel();
            view.OnRefresh += Call_OnRefresh;
            this.DataContext = view;

            refreshTimer = new System.Windows.Threading.DispatcherTimer();
            refreshTimer.Tick += new EventHandler(view.generate_schedule);
            refreshTimer.Interval = new TimeSpan(0, 0, (int)view.RefreshInterval);

        }

        void Call_OnRefresh(object s, EventArgs e)
        {
            if (!Models.EngineManagerModel.instance.IsConnected)
                return;
            if (view.schedule == null)
                return;
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            view.generate_schedule(this, null);
            refreshTimer.Interval = new TimeSpan(0, 0, (int)view.RefreshInterval);
            refreshTimer.Start();
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            refreshTimer.Stop();
        }

        private void RefreshTimer_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            refreshTimer.Interval = new TimeSpan(0, 0, (int)view.RefreshInterval);
        }
    }
}
