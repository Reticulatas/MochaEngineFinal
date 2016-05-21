using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    /// <summary>
    /// Defines a control that has a layout rect, but no visual.
    /// See Control for visual adherence
    /// </summary>
    public abstract class Element : ILayoutable
    {
        public abstract void DoLayout();
        public abstract void FinalizeLayout();

        public float width
        {
            get;
            set;
        }

        public float height
        {
            get;
            set;
        }

        public float x
        {
            get;
            set;
        }

        public float y
        {
            get;
            set;
        }
    }
}
