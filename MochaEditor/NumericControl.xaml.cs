using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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
    /// Interaction logic for NumericControl.xaml
    /// </summary>
    public partial class NumericControl : UserControl
    {
        public enum NumericControlType { INTEGER, DECIMAL };
        public NumericControlType Type { get; set; }

        public float _value;
        public float Value
        {
            get { return _value; }
            set
            {
                _value = value;
                NumericControlBox.Text = _value.ToString();
            }
        }


        private static bool IsTextAllowed(string text)
        {
            Regex regex = new Regex("[^0-9.-]+"); //regex that matches disallowed text
            return !regex.IsMatch(text);
        }

        public NumericControl()
        {
            InitializeComponent();
        }

        private void NumericControl_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            e.Handled = !IsTextAllowed(e.Text);
        }

        private void NumericControl_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
        }

        private void NumericControl_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {

        }

        private void NumericControl_TextChanged(object sender, TextChangedEventArgs e)
        {
            Value = Convert.ToSingle(NumericControlBox.Text);
        }
    }
}
