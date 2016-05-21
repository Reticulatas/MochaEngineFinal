using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Xna.Framework;

namespace MochaCore.MUI
{
    /// <summary>
    /// Surface is considered a top-end control
    /// </summary>
    public class Surface : ContainerControl
    {
        #region PROPERTIES
        bool _worldSpace;
        public bool WorldSpace
        {
            get { return _worldSpace; }
            set { _worldSpace = value; DoLayout(); }
        }

        Vector3 _worldposition;
        public Vector3 WorldPosition
        {
            get { return _worldposition; }
            set
            {
                _worldposition = value;
                DoLayout();
            }
        }
        #endregion

        protected override void Initialize()
        {
            base.Initialize();

            Name = "Surface";
        }

        public override void DoLayout()
        {
            //calculate width and height from the window
            width = MochaInterface.Engine.getInstance().GetGameWnd().GetWindowWidth();
            height = MochaInterface.Engine.getInstance().GetGameWnd().GetWindowHeight();

            base.DoLayout();

            FinalizeLayout();
        }

        public override void FinalizeLayout()
        {
            if (_worldSpace)
            {
                GetUnderlyingObject().SetTransform(WorldPosition, new Vector2(width, height));
            }
            else
            {
                GetUnderlyingObject().SetTransform(new Vector3(0, 0, 0), new Vector2(width, height));
            }
            base.FinalizeLayout();
        }

        protected override void OnChildAdded(Control child)
        {
            base.OnChildAdded(child);
        }

        protected override void OnChildRemoved(Control child)
        {
            base.OnChildRemoved(child);
        }
    }
}
