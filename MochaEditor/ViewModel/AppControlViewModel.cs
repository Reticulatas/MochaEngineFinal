using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace MochaEditor
{
    public class AppControlViewModel
    {
        AppControlModel _model;

        public event EventHandler OnAppOpenedSuccessfully;
        public event EventHandler OnAppExited;

        public bool ConnectExisting
        {
            get { return _model.ConnectExisting; }
            set { _model.ConnectExisting = value; }
        }

        public string ExeName
        {
            get
            {
                return _model.ExeName;
            }
            set
            {
                _model.ExeName = value;
            }
        }

        public bool IsAppInFocus
        {
            get
            {
                return _model.IsAppInFocus();
            }
        }

        public bool IsEmbedded { get { return _model.IsEmbedded; } set { _model.IsEmbedded = value; } }

        public AppControlViewModel()
        {
            _model = new AppControlModel();
            _model.OnAppExited += OnAppExited;
            _model.OnAppOpenedSuccessfully += new EventHandler(Route_OnAppOpenedSuccessfully);
        }

        void Route_OnAppOpenedSuccessfully(object s, EventArgs e)
        {
            if (OnAppOpenedSuccessfully != null)
                OnAppOpenedSuccessfully(s, e);
        }

        public void OnDisposed(bool disposing)
        {
            _model.OnDisposed(disposing);
        }

        public void OnResize(object s, SizeChangedEventArgs e, Point p)
        {
            _model.OnResize(s, e, p);
        }

        public void Recreate(object s)
        {
            _model.Recreate(s);
        }

        public void OnVisibleChanged(object s, DependencyPropertyChangedEventArgs e)
        {
            _model.OnVisibleChanged(s, e);
        }

        public void Embed(object s) { _model.Embed(s); }
        public void UnEmbed() { _model.UnEmbed(); }

        
    }
}
