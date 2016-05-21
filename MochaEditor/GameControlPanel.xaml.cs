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
using Microsoft.Win32;
using System.IO;
using MochaInterface;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for GameControlPanel.xaml
    /// </summary>
    public class LevelNameShortener : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            string s = value as string;
            return System.IO.Path.GetFileName(s);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    public partial class GameControlPanel : UserControl, INotifyPropertyChanged
    {
        //settings
        //render settings
        ColorConverter colorConv = new ColorConverter();
        public System.Windows.Media.Color ambientColor
        {
            get
            {
                System.Windows.Media.Color col = new System.Windows.Media.Color();
                col.ScR = Renderer.getInstance().mGlobalAmbient.x;
                col.ScG = Renderer.getInstance().mGlobalAmbient.y;
                col.ScB = Renderer.getInstance().mGlobalAmbient.z;
                col.ScA = Renderer.getInstance().mGlobalAmbient.w;
                return col;
            }
            set
            {
                Renderer.getInstance().mGlobalAmbient.x = value.ScR;
                Renderer.getInstance().mGlobalAmbient.y = value.ScG;
                Renderer.getInstance().mGlobalAmbient.z = value.ScB;
                Renderer.getInstance().mGlobalAmbient.w = value.ScA;
            }
        }

        public string _fps;
        public string fps
        {
            get { return _fps; }
            set
            {
                _fps = value;
            }
        }

        public int GridSize { get { return EngineCommandBindings.GridSize; } set { EngineCommandBindings.GridSize = value; } }
        public bool DrawGrid { get { return EngineCommandBindings.DrawGrid; } set { EngineCommandBindings.DrawGrid = value; } }
        public bool LetterBox { get { return Engine.getInstance().mIsLetterBox; } set { Engine.getInstance().mIsLetterBox = value; } }
        ///

        public GameControlPanel()
        {
            GridSize = 10;
            InitializeComponent();

            MochaScriptSystem.ScriptSubsystemManaged.HasReloaded += ScriptSubsystemManaged_HasReloaded;
        }

        void ScriptSubsystemManaged_HasReloaded(object sender, EventArgs e)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                GameControlPanelMain.IsEnabled = !MochaScriptSystem.ScriptSubsystemManaged.getInstance().HasErrors;
                if (!MochaScriptSystem.ScriptSubsystemManaged.getInstance().HasErrors)
                    ErrorText.Visibility = System.Windows.Visibility.Collapsed;
                else
                    ErrorText.Visibility = System.Windows.Visibility.Visible;
            }));
        }

        private void SaveMapButton_Click(object sender, RoutedEventArgs e)
        {
            EngineCommandBindings.CMD_SaveMap();
        }

        private void SaveMapAsButton_Click(object sender, RoutedEventArgs e)
        {
            EngineCommandBindings.CMD_SaveMapAs();
        }

        private void ForceLoadMapButton_Click(object sender, RoutedEventArgs e)
        {
            EngineCommandBindings.CMD_LoadMap();
        }

        private void MergeLevelButton_Click(object sender, RoutedEventArgs e)
        {
            EngineCommandBindings.CMD_MergeMap();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        private void ForceLinkButton_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
