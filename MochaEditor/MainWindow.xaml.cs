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
using System.Windows.Forms;
using FirstFloor.ModernUI.Windows.Controls;
using FirstFloor.ModernUI.Presentation;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>

    public partial class MainWindow : ModernWindow
    {

        public LinkCollection links;
        public static MainWindow instance;
        int currentLink = 0;

        public MainWindow()
        {
            InitializeComponent();

            Style = (Style)FindResource(typeof(ModernWindow));

            links = this.MenuLinkGroups[0].Links;
            instance = this;

        }

        private void Window_Closed(object sender, EventArgs e)
        {
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.ContentSource = this.MenuLinkGroups.First().Links.First().Source;
        }

        static bool loaded = false;
        private void Window_Initialized(object sender, EventArgs e)
        {
            if (!loaded)
            {
                //workspaceGroup.AddWorkspace(new Workspace_World());
                //workspaceGroup.AddWorkspace(new Workspace_World());
                loaded = true;
            }
        }

        public void NextLink()
        {
            if (currentLink < links.Count - 1)
                currentLink++;
            else
                currentLink = 0;
            this.ContentSource = links[currentLink].Source;
        }
        public void PrevLink()
        {
            if (currentLink > 0)
                currentLink--;
            else
                currentLink = links.Count - 1;
            this.ContentSource = links[currentLink].Source;
        }
        public void GoToLink(int i)
        {
            currentLink = i;
            this.ContentSource = links[currentLink].Source;
        }


        private void ModernWindow_KeyDown_1(object sender, System.Windows.Input.KeyEventArgs args)
        {
            if (Keyboard.Modifiers == ModifierKeys.Alt)
            {
                if (args.Key == Key.Right)
                    NextLink();
                if (args.Key == Key.Left)
                    PrevLink();
            }
        }

        public void NewLink(string displayname, string source, bool gotoLink = true)
        {
            //does it exist already
            var prel = from link in this.MenuLinkGroups[0].Links where link.DisplayName == displayname select link;
            if (prel.Count() != 0)
            {
                GoToLink(this.MenuLinkGroups[0].Links.IndexOf(prel.First()));
                return;
            }

            Link l = new Link();
            l.Source = new System.Uri(source, UriKind.RelativeOrAbsolute);
            l.DisplayName = displayname;

            this.MenuLinkGroups[0].Links.Add(l);

            links = this.MenuLinkGroups[0].Links;
            if (gotoLink)
                GoToLink(links.Count-1);
        }

        public void RemoveLink(string displayname)
        {
            var l = from link in this.MenuLinkGroups[0].Links where link.DisplayName == displayname select link;
            if (l == null)
                throw new KeyNotFoundException();
            this.MenuLinkGroups[0].Links.Remove(l.First());
            links = this.MenuLinkGroups[0].Links;
        }

        private void SnapToGrid_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            EngineCommandBindings.CMD_SnapToGrid();
        }
    }
}
