using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using FirstFloor.ModernUI.Presentation;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        

        private void StartupHandler(object sender, System.Windows.StartupEventArgs e)
        {
            //EngineManagerViewModel.instance._isFocused = true;
            MCommand.MCommands.BindCommands();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            AppearanceManager.Current.AccentColor = System.Windows.Media.Color.FromRgb(0, 89, 45);
        }

        private void OnWindowFocused(object sender, EventArgs e)
        {
            if (EngineManagerViewModel.instance != null)
            {
                MCommand.MCommands.RegisterAll();
            }
            //Console.WriteLine("ACTIVE");
            //Console.WriteLine("DX WINDOW " + MochaInterface.Engine.getInstance().GetGameWnd().GetFocus().ToString());
        }

        private void OnWindowUnfocused(object sender, EventArgs e)
        {
            if (EngineManagerViewModel.instance != null)
            {
                MCommand.MCommands.UnregisterAll();
            }
            //Console.WriteLine("INACTIVE");
            //Console.WriteLine("DX WINDOW " + MochaInterface.Engine.getInstance().GetGameWnd().GetFocus().ToString());
        }
    }
}
