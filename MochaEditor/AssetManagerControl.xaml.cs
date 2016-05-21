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
    /// Interaction logic for AssetManagerControl.xaml
    /// </summary>
    public partial class AssetManagerControl : UserControl
    {
        public AssetManagerControl()
        {
            InitializeComponent();

            //foreach (FirstFloor.ModernUI.Presentation.Link l in AssetTabsControl.Links)
            //{
             //   l.Source = new Uri(String.Format("AssetFilterViewControl.xaml?{1}#{0}", l.DisplayName, i++), UriKind.Relative);
           //}
        }
    }
}
