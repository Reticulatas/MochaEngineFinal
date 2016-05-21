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
    class AppControlModel
    {
        [System.Runtime.InteropServices.StructLayoutAttribute(System.Runtime.InteropServices.LayoutKind.Sequential)]
        public struct HWND__
        {

            /// int
            public int unused;
        }

        public event EventHandler OnAppOpenedSuccessfully;
        public event EventHandler OnAppExited;
        //public event EventHandler OnAppFocus;
        /// <summary>
        /// Track if the application has been created
        /// </summary>
        private bool _iscreated = false;

        /// <summary>
        /// Track if the control is disposed
        /// </summary>
        private bool _isdisposed = false;

        /// <summary>
        /// Handle to the application Window
        /// </summary>
        IntPtr _appWin;

        private Process _childp;

        /// <summary>
        /// The name of the exe to launch
        /// </summary>
        private string exeName = "";

        /// <summary>
        /// Whether or not to search for a currently running process to attach
        /// </summary>
        private bool connectExisting = false;
        private bool _windowFocus = false;
        public bool ConnectExisting
        {
            get { return connectExisting; }
            set { connectExisting = value; }
        }

        public string ExeName
        {
            get
            {
                return exeName;
            }
            set
            {
                exeName = value;
            }
        }

        private bool isembedded = false;
        public bool IsEmbedded { get { return isembedded; } set { isembedded = value; } }

        [DllImport("user32.dll", EntryPoint = "GetWindowThreadProcessId", SetLastError = true,
              CharSet = CharSet.Unicode, ExactSpelling = true,
              CallingConvention = CallingConvention.StdCall)]
        private static extern long GetWindowThreadProcessId(long hWnd, long lpdwProcessId);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern long SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll", EntryPoint = "GetWindowLongA", SetLastError = true)]
        private static extern long GetWindowLong(IntPtr hwnd, int nIndex);

        [DllImport("user32.dll", EntryPoint = "SetWindowLongA", SetLastError = true)]
        public static extern int SetWindowLongA([System.Runtime.InteropServices.InAttribute()] System.IntPtr hWnd, int nIndex, int dwNewLong);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern long SetWindowPos(IntPtr hwnd, long hWndInsertAfter, long x, long y, long cx, long cy, long wFlags);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool MoveWindow(IntPtr hwnd, int x, int y, int cx, int cy, bool repaint);

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        private static extern IntPtr GetFocus();

        [DllImport("user32.dll")]
        private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        protected delegate bool EnumThreadDelegate(IntPtr hWnd, IntPtr lParam);

        [DllImport("user32.dll")]
        static extern bool EnumThreadWindows(int dwThreadId, EnumThreadDelegate lpfn,
            IntPtr lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr SendMessageTimeout(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam, uint fuFlags, uint uTimeout, out IntPtr lpdwResult);

        private static string GetWindowTitle(IntPtr windowHandle)
        {
            uint SMTO_ABORTIFHUNG = 0x0002;
            uint WM_GETTEXT = 0xD;
            int MAX_STRING_SIZE = 32768;
            IntPtr result;
            string title = string.Empty;
            IntPtr memoryHandle = Marshal.AllocCoTaskMem(MAX_STRING_SIZE);
            Marshal.Copy(title.ToCharArray(), 0, memoryHandle, title.Length);
            SendMessageTimeout(windowHandle, WM_GETTEXT, (IntPtr)MAX_STRING_SIZE, memoryHandle, SMTO_ABORTIFHUNG, (uint)1000, out result);
            title = Marshal.PtrToStringAuto(memoryHandle);
            Marshal.FreeCoTaskMem(memoryHandle);
            return title;
        }

        static IEnumerable<IntPtr> EnumerateProcessWindowHandles(int processId)
        {
            var handles = new List<IntPtr>();

            foreach (ProcessThread thread in Process.GetProcessById(processId).Threads)
                EnumThreadWindows(thread.Id,
                    (hWnd, lParam) => { handles.Add(hWnd); return true; }, IntPtr.Zero);

            return handles;
        }

        /// <summary>
        /// Win32 API Constants for ShowWindowAsync()
        /// </summary>
        private const int SW_HIDE = 0;
        private const int SW_SHOWNORMAL = 1;
        private const int SW_SHOWMINIMIZED = 2;
        private const int SW_SHOWMAXIMIZED = 3;
        private const int SW_SHOWNOACTIVATE = 4;
        private const int SW_RESTORE = 9;
        private const int SW_SHOWDEFAULT = 10;

        private const int SWP_NOOWNERZORDER = 0x200;
        private const int SWP_NOREDRAW = 0x8;
        private const int SWP_NOZORDER = 0x4;
        private const int SWP_SHOWWINDOW = 0x0040;
        private const int WS_EX_MDICHILD = 0x40;
        private const int SWP_FRAMECHANGED = 0x20;
        private const int SWP_NOACTIVATE = 0x10;
        private const int SWP_ASYNCWINDOWPOS = 0x4000;
        private const int SWP_NOMOVE = 0x2;
        private const int SWP_NOSIZE = 0x1;
        private const int GWL_STYLE = (-16);
        private const int WS_VISIBLE = 0x10000000;
        private const int WS_CHILD = 0x40000000;

        public void OnVisibleChanged(object s, DependencyPropertyChangedEventArgs e)
        {
            if (!(s as UserControl).IsVisible)
                ShowWindow(_appWin, SW_HIDE);
            else
                ShowWindow(_appWin, SW_SHOWNORMAL);
        }

        void OnAppProcessExited(object e, object p)
        {
            OnAppExited(null, null);
        }

        public bool IsAppInFocus()
        {
            return (GetFocus() == _appWin);
        }

        /// <summary>
        /// Create control when visibility changes
        /// </summary>
        /// <param name="e">Not used</param>
        public void CreateApp(object sender)
        {
            // If control needs to be initialized/created
            if (_iscreated == false)
            {

                // Mark that control is created
                _iscreated = true;

                // Initialize handle value to invalid
                _appWin = IntPtr.Zero;

                try
                {
                    var procInfo = new System.Diagnostics.ProcessStartInfo(this.exeName);
                    procInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(this.exeName);

                    IEnumerable<IntPtr> ptr = EnumerateProcessWindowHandles(Process.GetCurrentProcess().Id);

                    _childp = Process.GetCurrentProcess();

                    _appWin = new IntPtr((int)MochaInterface.Engine.getInstance().GetGameWnd().GetHwndAsInt());

                    _childp.EnableRaisingEvents = true;
                    _childp.Exited += new EventHandler(OnAppProcessExited);
                }
                catch (Exception ex)
                {
                    Debug.Print(ex.Message + "Error");
                }

                // Put it into this form
                var helper = new WindowInteropHelper(Window.GetWindow((sender as AppControl).AppContainer));
                SetParent(_appWin, helper.Handle);
                //helper.Owner.
                // Remove border and whatnot
                SetWindowLongA(_appWin, GWL_STYLE, WS_VISIBLE);

                // Move the window to overlay it on this window
                var point = (sender as FrameworkElement).TranslatePoint(new Point(0, 0), App.Current.MainWindow);
                OnResize(sender, null, point);

                OnAppOpenedSuccessfully(this, null);
                
                //Window.GetWindow((sender as AppControl).AppContainer).Activated += new EventHandler(OnFocus);
                //Window.GetWindow((sender as AppControl).AppContainer).Deactivated += new EventHandler(OnUnfocus);
            }
        }

        public void UnEmbed()
        {
            SetParent(_appWin, System.IntPtr.Zero);
            IsEmbedded = false;
        }
        public void Embed(object s)
        {
            // Put it into this form
            var helper = new WindowInteropHelper(Window.GetWindow((s as AppControl).AppContainer));
            SetParent(_appWin, helper.Handle);
            IsEmbedded = true;
            
            //Window.
        }

        /// <summary>
        /// Update display of the executable
        /// </summary>
        /// <param name="e">Not used</param>
        public void OnResize(object s, SizeChangedEventArgs e, Point point)
        {
            if (this._appWin != IntPtr.Zero)
            {
                EditorSubsystemManaged.getInstance().QueueAction(() => MoveWindow(_appWin, (int)point.X, (int)point.Y, (int)(s as UserControl).ActualWidth, (int)(s as UserControl).ActualHeight, true));
            }
        }

        public void OnDisposed(bool disposing)
        {
            if (!_isdisposed)
            {
                if (disposing)
                {
                    if (_iscreated && _appWin != IntPtr.Zero && !_childp.HasExited)
                    {
                        // Stop the application
                        _childp.Close();

                        // Clear internal handle
                        _appWin = IntPtr.Zero;
                    }
                }
                _isdisposed = true;
            }
        }

        public void Recreate(object s)
        {
            (s as AppControl).Dispose(true);
            _iscreated = false;
            _isdisposed = false;

            CreateApp(s);
        }

        void OnFocus(object e, object p)
        {
            //IsAppInFocus()
            _windowFocus = true;
           // _windowFocus = !_windowFocus || EngineManagerVi;ewModel.instance._isFocused;
            //Console.WriteLine("DX WINDOW " + MochaInterface.Engine.getInstance().IsGameInFocus().ToString());
        }
        void OnUnfocus(object e, object p)
        {
            _windowFocus = false;
            // _windowFocus = !_windowFocus || EngineManagerViewModel.instance._isFocused;
            //MochaInterface.Engine.getInstance()
            //Console.WriteLine("DX WINDOW " + MochaInterface.Engine.getInstance().IsGameInFocus().ToString());
        }
    }
}
