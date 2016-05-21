using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    public class StackPanel : ContainerControl 
    {
#region PROPERTIES
        public enum ORIENTATION
        {
            VERTICAL, HORIZONTAL
        }

        ORIENTATION _orientation;
        public ORIENTATION Orientation { get { return _orientation; } set { _orientation = value; } }
#endregion

        public override void DoLayout()
        {
            float positionChange = 0;
            foreach (Control child in Children)
            {
                if (_orientation == ORIENTATION.VERTICAL)
                {
                    child.y = positionChange;
                    positionChange += child.height;
                }
                else
                {
                    child.x = positionChange;
                    positionChange += child.width;
                }
            }

            base.DoLayout();    //do children layout
        }
    }
}
