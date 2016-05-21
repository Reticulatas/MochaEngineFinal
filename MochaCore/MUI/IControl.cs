using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    interface IControl
    {
        string Name { get; set; }

        void DoLayout();
    }
}
