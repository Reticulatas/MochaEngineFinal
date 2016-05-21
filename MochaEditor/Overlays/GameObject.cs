using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

public class GameObject : MochaInterface.GameObject, IComparable, INotifyPropertyChanged
{
    GameObject() { throw new InvalidOperationException("Cannot make a game object"); }
    public GameObject(MochaInterface.GameObject g)
        : base(g.get_CPtr().Handle)
    {

    }

    public string Name
    {
        get
        {
            return mName;
        }
        set
        {
            mName = value;
            OnPropertyChanged("Name");
        }
    }

    public bool IsLinkedToPrefab
    {
        get
        {
            return (linkedPrefab != null);
        }
    }

    public int CompareTo(object obj)
    {
        GameObject other = obj as GameObject;
        if (other == null)
            return -1;

        return other.mName.CompareTo(this.mName);
    }

    public event EventHandler<bool> OnSelectionChanged;
    public bool IsSelected
    {
        get
        {
            return mIsSelected;
        }
        set
        {
            mIsSelected = value;
            /*if (!_lockMyself)
            {
                _lockMyself = true;
                if (_isSelected)
                {
                    GameObjectSelectionManager.RequestAdditiveSelect(this);
                }
                else
                {
                    GameObjectSelectionManager.RequestUnSelect(this);
                }
                _lockMyself = false;
            }*/
            OnPropertyChanged("IsSelected");
            if (OnSelectionChanged != null)
                OnSelectionChanged(this, mIsSelected);
        }
    }

    public static GameObject From(MochaInterface.GameObject b)  // explicit byte to digit conversion operator
    {
        if (b == null)
            return null;
        GameObject d = new GameObject(b);  // explicit conversion
        return d;
    }

    #region PROPERTYCHANGED
    public event PropertyChangedEventHandler PropertyChanged;

    public void OnPropertyChanged(string name)
    {
        PropertyChangedEventHandler handler = PropertyChanged;
        if (handler != null)
        {
            handler(this, new PropertyChangedEventArgs(name));
        }
    }
    #endregion
}