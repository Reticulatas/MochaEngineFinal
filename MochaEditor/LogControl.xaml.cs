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

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for LogControl.xaml
    /// </summary>
    public partial class LogControl : UserControl
    {
        public ObservableCollection<LogEntry> logs
        {
            get
            {
                return Logger.logs;
            }
            set
            {
                Logger.logs = value;
            }
        }

        public LogControl()
        {
            InitializeComponent();


        }

        void LogControl_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            DataGrid_AddingNewItem(null, null);
        }

        private void DataGrid_AddingNewItem(object sender, AddingNewItemEventArgs e)
        {
            try
            {
                if (ConsoleDataGrid.Items.Count > 0)
                {
                    var border = VisualTreeHelper.GetChild(ConsoleDataGrid, 0) as Decorator;
                    if (border != null)
                    {
                        var scroll = border.Child as ScrollViewer;
                        if (scroll != null) scroll.ScrollToEnd();
                    }
                }
            }catch(ArgumentOutOfRangeException)
            {
                //consume naively
            }
        }

        private void ClearButton_Click(object sender, RoutedEventArgs e)
        {
            logs.Clear();
        }

        private void ConsoleDataGrid_Loaded(object sender, RoutedEventArgs e)
        {
            (ConsoleDataGrid.ItemsSource as ObservableCollection<LogEntry>).CollectionChanged += LogControl_CollectionChanged;
        }
    }
}
