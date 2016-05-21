using System;
using System.Windows;
using System.Windows.Controls;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Windows.Interop;
using System.Windows.Media;
using System.Collections.Generic;
using System.Threading;

namespace MochaEditor 
{
    /// <summary>
    /// AppControl.xaml 
    /// </summary>
    public partial class AppControl : UserControl, IDisposable
    {
        //List<string> temp;
        public AppControl()
        {
            _viewModel = new AppControlViewModel();
            InitializeComponent();
            this.SizeChanged += new SizeChangedEventHandler(OnSizeChanged);
            this.SizeChanged += new SizeChangedEventHandler(OnResize);
            this.IsVisibleChanged += new DependencyPropertyChangedEventHandler(OnVisibleChanged);
            topWin = (GetTopLevelControl(this) as Window);

        }

        public bool IsAppInFocus()
        {
            return _viewModel.IsAppInFocus;
        }
        

        private void OnVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            _viewModel.OnVisibleChanged(sender, e);
            EngineManagerViewModel.instance.EngineEmbedHasVisibility = (bool)e.NewValue;
        }

        public void OnResize(object sender, SizeChangedEventArgs e)
        {
            var point = this.TranslatePoint(new Point(0, 0), topWin);
            _viewModel.OnResize(sender, e, point);
        }

        DependencyObject GetTopLevelControl(DependencyObject control)
        {
            DependencyObject tmp = control;
            DependencyObject parent = null;
            while ((tmp = VisualTreeHelper.GetParent(tmp)) != null)
            {
                parent = tmp;
            }
            return parent;
        }

        ~AppControl()
        {
            this.Dispose();
        }

        Window topWin;
        AppControlViewModel _viewModel;
        public AppControlViewModel ViewModel { get { return _viewModel; } }

        public virtual void Dispose(bool disposing)
        {
            _viewModel.OnDisposed(disposing);
        }

        protected void OnSizeChanged(object s, SizeChangedEventArgs e)
        {
            this.InvalidateVisual();
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        public void Recreate()
        {
            _viewModel.Recreate(this);
        }

    }
}
