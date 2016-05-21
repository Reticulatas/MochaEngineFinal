using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    public class ContainerControl : Control, IContainer
    {
        List<Control> _children;
        public IEnumerable<Control> Children { get { return _children; } set { _children = (List<Control>)value; } }

        #region OVERRIDES
        protected override void Initialize()
        {
            base.Initialize();

            _children = new List<Control>();
        }

        public override void DoLayout()
        {
            base.DoLayout();

            foreach (Control control in Children)
                control.DoLayout();

            FinalizeLayout();
        }
        public override void FinalizeLayout()
        {
            base.FinalizeLayout();
            foreach (Control control in Children)
                control.FinalizeLayout();
        }
        #endregion

        #region INTERNAL
        protected void AddChild(Control child)
        {
            GetUnderlyingObject().AddChild(child.GetUnderlyingObject());
            _children.Add(child);
            OnChildAdded(child);
        }
        protected void RemoveChild(Control child)
        {
            GetUnderlyingObject().RemoveChild(child.GetUnderlyingObject());
            _children.Remove(child);
            OnChildRemoved(child);
        }
        #endregion


        /* Override these */
        #region USER
        protected virtual void OnChildAdded(Control child)
        {
            DoLayout();
        }

        protected virtual void OnChildRemoved(Control child)
        {
            DoLayout();
        }

        #endregion
    }
}
