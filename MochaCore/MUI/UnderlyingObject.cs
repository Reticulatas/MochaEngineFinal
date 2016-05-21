using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Xna.Framework;

namespace MochaCore.MUI
{
    public class UnderlyingObject
    {
        #region PROPERTIES
        MochaInterface.CDiffuseRenderer _renderer;
        public MochaInterface.CDiffuseRenderer Renderer 
        {
            get {
                if (_renderer == null)
                    throw new MUIException("Underlying has not been initialized.");
                return _renderer;
            }
            set { _renderer = value; }
        }

        MochaInterface.GameObject _gameObject;
        public MochaInterface.GameObject GameObject 
        {
            get { 
            if (_gameObject == null)
                throw new MUIException("Underlying has not been initialized.");
                return _gameObject;
            }
            set { _gameObject = value; }
        }
        #endregion

        public UnderlyingObject()
        {
            _gameObject = MochaInterface.ObjectFactory.getInstance().MakeNewGameObject("Control");
            _renderer = _gameObject.RequireComponent<MochaInterface.CDiffuseRenderer>();
        }

        #region ACCESSORS
        public void SetName(string name)
        {
            GameObject.SetName(name);
        }
        public void SetTransform(Vector3 position, Vector2 scale)
        {
            GameObject.transform.SetPosition(position);
        }
        public void AddChild(UnderlyingObject obj)
        {
            obj.GameObject.transform.SetParent(GameObject.transform);
        }
        public void RemoveChild(UnderlyingObject obj)
        {
            obj.GameObject.transform.SetParent(null);
        }
        #endregion
    }
}
