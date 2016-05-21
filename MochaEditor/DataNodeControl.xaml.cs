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
using Microsoft.Xna.Framework;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for DataNodeControl.xaml
    /// </summary>
    public partial class DataNodeControl : UserControl
    {
        public DataNodeControl()
        {
            InitializeComponent();
        }

        private void SendDataChange()
        {
        }

        private void PS_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            SendDataChange();
        }

        private void PS_TextChanged(object sender, TextChangedEventArgs e)
        {
            SendDataChange();
        }

        private void PS_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color> e)
        {
            SendDataChange();
        }

        bool loaded = false;
        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (!loaded)
            {
                loaded = true;

                if (DataContext == null)
                {
                    WPFHelper.InvalidateControl(this, "Value Null");
                    return;
                }

                //get the bound object type's sanitized name
                string typetype = (string)DataPropertyTypeDeducer.Instance.Convert((DataContext as PropertyValueConverter.PropertyInfo).PropertyValue, (DataContext as PropertyValueConverter.PropertyInfo).PropertyType, null, null);
                if (typetype == null)
                {
                    DataControlPanel.Content = (this.FindResource("PS_NULLPTR") as DataTemplate).LoadContent();
                    return;
                }

                //find corresponding PS template
                DataTemplate template = null;
                try
                {
                    if (typetype.Contains("Container"))
                    {
                        string underlyingtypename = typetype.Split('|').Last();
                        typetype = "Container";
                    }
                    template = this.FindResource("PS_" + typetype) as DataTemplate;
                }
                catch (System.Windows.ResourceReferenceKeyNotFoundException)
                {
                    //could not find a ps template, try to recurse properties
                    template = this.FindResource("PS_RecurseProps") as DataTemplate;
                    //DataControlPanel.MinHeight = ps.Height;

                    //WPFHelper.InvalidateControl(this, "[" + typetype + "] type inv.");
                }

                DataControlPanel.Content = template.LoadContent();
            }
        }

        private void PS_Click(object sender, RoutedEventArgs e)
        {
                SendDataChange();
        }

        private void PS_NULLPTR_BUTTON_Click(object sender, RoutedEventArgs e)
        {
            if (MessageBox.Show("Use Default Constructor to Create Object?", "Initialize", MessageBoxButton.YesNo) == MessageBoxResult.Yes)
            {
                PropertyValueConverter.PropertyInfo pi = DataContext as PropertyValueConverter.PropertyInfo;
                try
                {
                    if (pi.PropertyType == typeof(string))
                        pi.PropertyValue = String.Empty;
                    else
                        pi.PropertyValue = pi.PropertyType.GetConstructor(System.Type.EmptyTypes).Invoke(null);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Could not create object\n" + ex.Message);
                }
            }
        }

        private void DecimalUpDown_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            //USED ONLY FOR VECTOR3 BINDS
            Vector3 v = (Vector3)(WPFHelper.FindParent<DataNodeControl>(sender as DependencyObject).DataContext as PropertyValueConverter.PropertyInfo)
                .PropertyValue;
            IEnumerable<float> comps = ((sender as FrameworkElement).DataContext as IEnumerable<float>);
            v.X = comps.ElementAt(0);
            v.Y = comps.ElementAt(1);
            v.Z = comps.ElementAt(2);
            (WPFHelper.FindParent<DataNodeControl>(sender as DependencyObject).DataContext as PropertyValueConverter.PropertyInfo)
                .PropertyValue = v;
        }
    }
}
