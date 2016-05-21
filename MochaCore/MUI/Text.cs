using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaCore.MUI
{
    public class Text : Control
    {
        #region PROPERTIES
        string _text = "Text";
        public string Content { get { return _text; } set { _text = value; } }
        uint _fontSize = 12;
        public uint FontSize { get { return _fontSize; } set { _fontSize = value; } }
        MochaInterface.Color _fontcolor = new MochaInterface.Color(1.0f, 1.0f, 1.0f);
        public MochaInterface.Color FontColor { get { return _fontcolor; } set { _fontcolor = value; } }
        float _fontScale = 0.1f;
        public float FontScale { get { return _fontScale; } set { _fontScale = value; } }

        MochaInterface.CFontRenderer _fontRenderer;
        #endregion

        public override void DoLayout()
        {
            _fontRenderer.mText = _text;
            _fontRenderer.mFontSize = _fontSize;
            _fontRenderer.mColor = _fontcolor;
            _fontRenderer.mFontScale = _fontScale;

            base.DoLayout();
        }

        protected override void Initialize()
        {
            base.Initialize();

            _fontRenderer = GetUnderlyingObject().GameObject.RequireComponent<MochaInterface.CFontRenderer>();
        }
    }
}
