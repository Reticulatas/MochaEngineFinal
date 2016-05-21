using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CSharp;
using Microsoft.Xna.Framework;
using System.CodeDom.Compiler;
using System.IO;
using MochaInterface;

namespace MochaScriptSystem 
{
    public class ScriptManager
    {
        CompilerResults _comp;

        public ScriptManager()
        {
            CompileAll();
        }

        public CompilerResults CompileAll()
        {
            Logger.Log("Scripts recompiling...");
            //compile all scripts 
            CSharpCodeProvider provider = new CSharpCodeProvider();
            CompilerParameters parameters = new CompilerParameters();
            parameters.ReferencedAssemblies.Add("MochaCore.dll");
            parameters.ReferencedAssemblies.Add("Microsoft.Xna.Framework.Math.dll");
            parameters.ReferencedAssemblies.Add("System.dll");
            parameters.GenerateExecutable = false;
            parameters.GenerateInMemory = false; //default
            parameters.TempFiles = new TempFileCollection(Environment.GetEnvironmentVariable("TEMP"), true);
            parameters.IncludeDebugInformation = true;
            //parameters.CompilerOptions += "/optimize";
            string[] filepaths = Directory.GetFiles("Assets\\Scripts\\", "*.cs", SearchOption.AllDirectories);
            for (int i = 0; i < filepaths.Count(); ++i )
                filepaths[i] = Directory.GetCurrentDirectory() + "\\" + filepaths[i];
            CompilerResults results = provider.CompileAssemblyFromFile(parameters, filepaths);
            if (results.Errors.HasErrors)
            {
                StringBuilder sb = new StringBuilder();
                foreach (CompilerError error in results.Errors)
                    sb.AppendLine(String.Format("Error in {0} at {1}: {2} ({3})", Path.GetFileNameWithoutExtension(error.FileName), error.Line, error.ErrorText, error.ErrorNumber));
                Logger.Log("Scripts recompiled with errors.");
                Logger.Log(sb.ToString());
                ScriptSubsystemManaged.getInstance().SetHasError(true);
                return null;
            }
            else
                ScriptSubsystemManaged.getInstance().SetHasError(false);

            _comp = results;

            Logger.Log("Scripts recompiled without error.");

            return results;
        }

        public object MakeObject(string objectname)
        {
            object obj = null;
            try
            {
                obj = Activator.CreateInstance(_comp.CompiledAssembly.GetType(objectname));
            }
            catch (NullReferenceException)
            {
                Logger.Log("Script Error: Object " + objectname + " does not exist, script containing this object was not found.");
                return null;
            }
            catch (Exception e)
            {
                Logger.Log("Script added is invalid: " + e.Message);
                return null;
            }
            return obj;
        }

        /// <summary>
        /// Singleton access
        /// </summary>
        private static ScriptManager inst = null;
        public static ScriptManager getInstance()
        {
            if (inst == null)
                inst = new ScriptManager();
            return inst;
        }
    }
}
