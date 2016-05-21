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
    /// Interaction logic for PS_Enum.xaml
    /// </summary>
    public partial class PS_Enum : UserControl
    {
        PropertyValueConverter.PropertyInfo enumInfo = null;

        public PS_Enum()
        {
            InitializeComponent();
        }

        private void EnumValueComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (enumInfo == null)
                throw new InvalidOperationException("Can't set enum before enum info is generated");

            int i = 0;
            foreach (string s in enumInfo.PropertyValue.GetType().GetEnumNames())
            {
                if (EnumValueComboBox.SelectedValue as string == s)
                    break;
                ++i;
            }
            Array vals = enumInfo.PropertyValue.GetType().GetEnumValues();
            if (i < vals.Length && i >= 0)
                enumInfo.PropertyValue = vals.GetValue(i);
            else
                enumInfo.PropertyValue = null;
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (this.DataContext == null)
                return;
            enumInfo = this.DataContext as PropertyValueConverter.PropertyInfo;
            EnumValueComboBox.SelectedItem = enumInfo.PropertyValue.GetType().GetEnumName(enumInfo.PropertyValue);
        }
    }
}
