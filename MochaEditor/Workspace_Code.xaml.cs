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
using ICSharpCode.AvalonEdit.Highlighting;
using ICSharpCode.AvalonEdit.Highlighting.Xshd;
using ICSharpCode.AvalonEdit.CodeCompletion;
using ICSharpCode.AvalonEdit.Document;
using ICSharpCode.AvalonEdit.Editing;
using System.Xml;
using System.ComponentModel;
using System.Reflection;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for Workspace_Code.xaml
    /// </summary>
    public partial class Workspace_Code : UserControl, INotifyPropertyChanged
    {
        bool loaded = false;
        CompletionWindow completionWindow;
        string _loadedFile, _statusText;
        public string loadedFile { get { return _loadedFile; } set { _loadedFile = value; OnPropertyChanged("loadedFile"); } }
        public string statusText { get { return _statusText; } set { _statusText = "[" + DateTime.Now.ToString() + "] " + value; OnPropertyChanged("statusText"); } }

        public Workspace_Code()
        {
            InitializeComponent();
        }
        public Workspace_Code(string script_filepath)
        {
            InitializeComponent();
            CodeEditor.Load(script_filepath);
        }

        private void codeEditor_TextEntering(object sender, TextCompositionEventArgs e)
        {
            if (e.Text.Length > 0 && completionWindow != null)
            {
                if (!char.IsLetterOrDigit(e.Text[0]))
                {
                    // Whenever a non-letter is typed while the completion window is open,
                    // insert the currently selected element.
                    completionWindow.CompletionList.RequestInsertion(e);
                }
            }
        }
        private void codeEditor_TextEntered(object sender, TextCompositionEventArgs e)
        {
            if (e.Text == ".")
            {
                ICSharpCode.AvalonEdit.Editing.TextArea area = (sender as ICSharpCode.AvalonEdit.Editing.TextArea);
                ICSharpCode.AvalonEdit.Document.TextDocument doc = area.Document;
                int startoff = area.Caret.Offset-1;
                int off = area.Caret.Offset-2;
                while (true)
                {
                    char o = doc.GetCharAt(off);
                    if (!char.IsLetter(o))
                        break;
                    if (off == 0)
                        break;
                    off--;
                }
                string prevToken = doc.GetText(off, startoff - off);

                // Open code completion after the user has pressed dot:
                completionWindow = new CompletionWindow(CodeEditor.TextArea);
                completionWindow.Margin = new Thickness(0);
                IList<ICompletionData> data = completionWindow.CompletionList.CompletionData;
                try
                {
                    List<string> items = CompletionDatabase.completionDatabase[prevToken];
                    foreach (string item in items)
                        data.Add(new CompletionData(item));
                    completionWindow.Show();
                    completionWindow.Closed += delegate
                    {
                        completionWindow = null;
                    };
                }catch(Exception)
                {

                }
            }
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (loaded)
                return;
            loaded = true;

            CodeEditor.SyntaxHighlighting = HighlightingLoader.Load(new XmlTextReader("external/CSharp-Mode.xshd"), HighlightingManager.Instance);
            CodeEditor.Foreground = new SolidColorBrush(System.Windows.Media.Color.FromRgb(170, 170, 170));

                            
            CodeEditor.TextArea.TextEntered += codeEditor_TextEntered;
            CodeEditor.TextArea.TextEntering += codeEditor_TextEntering;
            CompletionDatabase.Generate();
        }

        private void CodeEditor_DragEnter(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent("assetFormat") || sender == e.Source)
            {
                e.Effects = DragDropEffects.None;
            }
        }

        private void CodeEditor_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("assetFormat"))
            {
                AssetItem asset = e.Data.GetData("assetFormat") as AssetItem;
                try
                {
                    CodeEditor.Load(asset.filepath);
                }catch(System.IO.IOException ex)
                {
                    WPFHelper.InvalidateControl(this, ex.Message);
                    return;
                }
                loadedFile = asset.filepath;
                statusText = "Loaded: " + loadedFile;
            }
        }

        private void CodeEditor_KeyDown(object sender, KeyEventArgs e)
        {
            if (loadedFile == null)
                return;
            if (Keyboard.Modifiers == ModifierKeys.Control)
            {
                if (e.Key == Key.S)
                {
                    CodeEditor.Save(loadedFile);
                    statusText = "Saved Script To: " + loadedFile;
                }
            }
        }

        private void OnPropertyChanged(string prop)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(prop));
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;
    }

    /// Implements AvalonEdit ICompletionData interface to provide the entries in the
    /// completion drop down.
    public class CompletionData : ICompletionData
    {
        public CompletionData(string text)
        {
            this.Text = text;
        }

        public System.Windows.Media.ImageSource Image
        {
            get { return null; }
        }

        public string Text { get; private set; }

        // Use this property if you want to show a fancy UIElement in the list.
        public object Content
        {
            get { return this.Text; }
        }

        public object Description
        {
            get { return "Description for " + this.Text; }
        }

        public double Priority { get { return 1.0f; } }

        public void Complete(TextArea textArea, ISegment completionSegment,
            EventArgs insertionRequestEventArgs)
        {
            textArea.Document.Replace(completionSegment, this.Text);
        }
    }

    public class CompletionDatabase
    {
        public static Dictionary<string,List<string>> completionDatabase = new Dictionary<string,List<string>>();
        public static void Generate()
        {
            completionDatabase.Clear();
            //first time generate name to type lookups
            var lListOfBs = (from lAssembly in AppDomain.CurrentDomain.GetAssemblies()
                             from lType in lAssembly.GetTypes()
                             where lAssembly.GetName().Name == "MochaDotNET"
                             where typeof(IDisposable).IsAssignableFrom(lType)
                             select lType).ToArray();
            foreach (Type t in lListOfBs)
            {
                if (completionDatabase.ContainsKey(t.Name))
                    continue;
                List<string> ss = new List<string>();
                foreach (MemberInfo t_m in t.GetMembers())
                    ss.Add(t_m.Name);
                foreach (PropertyInfo t_p in t.GetProperties())
                    ss.Add(t_p.Name);
                foreach (MethodInfo t_me in t.GetMethods())
                    ss.Add(t_me.Name);
                completionDatabase.Add(t.Name, ss);
            }
        }
    }
}
