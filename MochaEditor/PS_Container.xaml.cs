using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
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
    /// Interaction logic for PS_Container.xaml
    /// </summary>
    public partial class PS_Container : UserControl
    {
        public PS_Container()
        {
            InitializeComponent();
        }

        ConstructorInfo ci;

        private void AddItemButton_Click(object sender, RoutedEventArgs e)
        {
            object obj = null;
            if (ci.GetParameters().Count() > 0)
                obj = ci.Invoke(new object[] {new char[] {' '}});
            else
                obj = ci.Invoke(null);
            MethodInfo mi = (this.DataContext.GetType().GetMethod("Add"));
            if (mi == null)
                WPFHelper.InvalidateControl(this, "No Add Function on Container");
            object cont = (this.DataContext);
            object[] prms = {obj};
            EditorSubsystemManaged.getInstance().QueueAction(() => { mi.Invoke(cont, prms); });
        }

        private void RemoveItemButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (this.DataContext == null)
                return;
            Type objtype = null;
            try
            {
                objtype = (this.DataContext as PropertyValueConverter.PropertyInfo).PropertyValue.GetType().GetMethod("Add").GetParameters().FirstOrDefault().ParameterType;
            }catch(AmbiguousMatchException)
            {
                //WPFHelper.InvalidateControl(this, ex.Message);
                    AddItemButton.IsEnabled = false;
                    AddItemButton.Content = "[no suitable add]";
                return;
            }
            ci = objtype.GetConstructor(Type.EmptyTypes);

            if (ci == null)
            {
                //last chance for strings
                Type[] types = {typeof(char[])};
                ci = objtype.GetConstructor(types);
                if (ci == null)
                {
                    AddItemButton.IsEnabled = false;
                    AddItemButton.Content = "[no default constructor]";
                }
            }
        }
    }
}
