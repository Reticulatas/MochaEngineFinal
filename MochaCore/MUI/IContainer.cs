using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    interface IContainer
    {
        IEnumerable<Control> Children { get; set; }
    }
}
