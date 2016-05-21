using System;
using System.Collections.Generic;
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

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for CommandMenuItem.xaml
    /// </summary>
    public partial class CommandMenuItem : UserControl, INotifyPropertyChanged
    {
        public CommandMenuItem()
        {
            InitializeComponent();
        }

        public string CommandKeyName { get; set; }

        public MCommand.MCommand bound_command { get; set; }

        private static void CommandKeyChangedCallback(DependencyObject obj,
            DependencyPropertyChangedEventArgs e)
        {
            try
            {
                CommandMenuItem item = obj as CommandMenuItem;
                item.bound_command = MCommand.MCommand.commands[e.NewValue as string];
                item.OnPropertyChanged("bound_command");
            }
            catch (Exception)
            {
                //throw new InvalidOperationException("Command Key Not Found: " + e.NewValue as string);
            }
        }

        public static readonly DependencyProperty CommandKeyNameProperty =
            DependencyProperty.Register("CommandKeyName", typeof(string), typeof(CommandMenuItem),
            new PropertyMetadata("UNASSIGNED", CommandKeyChangedCallback));

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

        private void UniformGrid_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            bound_command.InvokeCommand();
            e.Handled = true;
        }
    }
}
