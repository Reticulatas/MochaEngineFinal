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
    /// Interaction logic for AssetsViewer.xaml
    /// </summary>
    public partial class AssetsViewer : UserControl
    {
        public AssetsViewer()
        {
            var itemsProvider = new AssetViewer.ItemProvider();
            var items = itemsProvider.GetItems("Assets");
            DataContext = items;

            InitializeComponent();
        }
    }
}
