using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    public class MUIException : Exception
    {
        public MUIException(string msg) : base(msg) { }
    }
}
