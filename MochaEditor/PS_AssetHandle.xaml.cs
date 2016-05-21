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
using MochaInterface;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for PS_AssetHandle.xaml
    /// </summary>
    public partial class PS_AssetHandle : UserControl
    {
        public PS_AssetHandle()
        {
            InitializeComponent();
        }

        private void Set_Handle(AssetHandle newhandle)
        {
            var info = (this.DataContext as PropertyValueConverter.PropertyInfo);
            System.Reflection.PropertyInfo i = info.PropertyObj.GetType().GetProperty(info.PropertyName);
            i.SetValue(info.PropertyObj, newhandle);
            info.PropertyValue = newhandle;
        }

        private void TextBlock_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("assetFormat"))
            {
                var thisHandle = (this.DataContext as PropertyValueConverter.PropertyInfo).PropertyValue as AssetHandle;

                AssetItem asset = e.Data.GetData("assetFormat") as AssetItem;
                if (asset.CanDropOn(thisHandle))
                {
                    e.Effects = DragDropEffects.None;
                    this.Effect = null;

                    EditorSubsystemManaged.getInstance().QueueAction(() => {
                        Dispatcher.Invoke(() =>
                        {
                            this.Set_Handle(AssetManager.getInstance().GetAsset(asset.fileid));
                            (sender as TextBlock).GetBindingExpression(TextBlock.TextProperty).UpdateTarget();
                        });
                    });
                }
            }
        }

        private void TextBlock_DragEnter(object sender, DragEventArgs e)
        {
            /*
            if (e.Data.GetDataPresent("assetFormat"))
            {
                var thisHandle = (this.DataContext as PropertyValueConverter.PropertyInfo).PropertyValue as AssetHandle;
                AssetItem asset = e.Data.GetData("assetFormat") as AssetItem;
                EditorSubsystemManaged.getInstance().QueueAction(() => {
                if (asset.CanDropOn(thisHandle))
                {
                    Dispatcher.Invoke(() =>
                    {
                    e.Effects = DragDropEffects.Move;
                    this.Effect = new System.Windows.Media.Effects.BlurEffect();
                    });
                }});
            }*/
        }

        private void TextBlock_DragLeave(object sender, DragEventArgs e)
        {
            e.Effects = DragDropEffects.None;
            this.Effect = null; 
        }

        private void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            var info = (this.DataContext as PropertyValueConverter.PropertyInfo);
            System.Reflection.PropertyInfo i = info.PropertyObj.GetType().GetProperty(info.PropertyName);
            //i.SetValue(info.PropertyObj, newhandle);
            (info.PropertyValue as AssetHandle).ResetHandle();
        }
    }
}
