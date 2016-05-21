using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    interface ILayoutable
    {
        float width {get; set;}
        float height { get; set; }
        //local space x,y
        float x {get; set;}
        float y {get; set;}
            
        void DoLayout();        //do positional computation
        void FinalizeLayout();  //commit layout to transform
    }
}
