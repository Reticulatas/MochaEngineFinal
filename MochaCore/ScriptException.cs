using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaScriptSystem 
{
    public class ScriptException : Exception
    {
        public ScriptException(string s) : base(s)
        {
        }
    }
}
