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

namespace MochaEditor.Controls
{
    /// <summary>
    /// Interaction logic for ModernModalWindow.xaml
    /// </summary>
    public partial class ModernModalWindow : ModernDialog
    {
        public string Message { get; set; }

        public ModernModalWindow(string message)
        {
            Message = message;
            InitializeComponent();
        }
    }
}
