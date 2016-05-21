using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace MochaCore
{
    public class CoffeeConstructor
    {
        static MochaScriptSystem.ScriptSubsystemManaged ss;

        static bool gameRunning = true;

        private static void Register()
        {
            if (!MochaInterface.Engine.getInstance().GetIsInited())
                throw new InvalidOperationException("Engine must be init'ed before registering script subsystem");
            ss = new MochaScriptSystem.ScriptSubsystemManaged();
            ss.RegisterToNative();
            MochaInterface.Engine.getInstance().LoadInitialLevel();
        }

        public static void StartFromConfig()
        {
            MochaInterface.Engine.getInstance().StartFromConfig(false);
            Register();
            MochaInterface.Engine.getInstance().mIsLetterBox = true;
        }
        public static void Start(bool showConsole, uint width, uint height, bool fullscreen)
        {
            MochaInterface.Engine.getInstance().Start(showConsole, width, height, fullscreen, false);
            Register();
        }

        public static void Run()
        {
            MochaInterface.Engine.getInstance().Run();
            MochaScriptSystem.ScriptSubsystemManaged.getInstance().Tick();
        }

        public static void Loop()
        {
            while (gameRunning)
            {
                Run();
            }
        }

        public static void Exit()
        {
            gameRunning = false;
            Process.GetCurrentProcess().Kill();
        }
    }
}
