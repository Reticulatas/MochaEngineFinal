using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Xna.Framework;

namespace MochaCore.MUI
{
    public class Control : Element, IControl
    {
        UnderlyingObject _underlying { get; set; }

        public UnderlyingObject GetUnderlyingObject() { return _underlying; }

        protected virtual void Initialize()
        {
            if (_initialized)
                throw new MUIException("Control already initialized.");

            _underlying = new UnderlyingObject();

            _initialized = true;
        }

#region PROPERTIES

        bool _initialized
        {
            get;
            set;
        }

        string _name;
        public string Name { get { return _name; } set { _underlying.SetName(_name); _name = value; } }
#endregion

        public override void DoLayout()
        {
        }

        public override void FinalizeLayout()
        {
            //lowest level of finalize layout - controls that make changes to layout transforms must invoke this down
            //finalize self - if control is a container, see ContainerControl for child finalization
            GetUnderlyingObject().SetTransform(new Vector3(x, y, 0), new Vector2(width, height));
        }
    }
}
