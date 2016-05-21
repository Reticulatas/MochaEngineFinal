/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace MochaInterface {

	using System;
	using System.Runtime.InteropServices;

	//using MochaDotNET;
	//using System.ComponentModel;

public partial class GameObjectVector : global::System.IDisposable, global::System.Collections.IEnumerable
    , global::System.Collections.Generic.IList<GameObject>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal GameObjectVector(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(GameObjectVector obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~GameObjectVector() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_GameObjectVector(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public GameObjectVector(global::System.Collections.ICollection c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (GameObject element in c) {
      this.Add(element);
    }
  }

  public bool IsFixedSize {
    get {
      return false;
    }
  }

  public bool IsReadOnly {
    get {
      return false;
    }
  }

  public GameObject this[int index]  {
    get {
      return getitem(index);
    }
    set {
      setitem(index, value);
    }
  }

  public int Capacity {
    get {
      return (int)capacity();
    }
    set {
      if (value < size())
        throw new global::System.ArgumentOutOfRangeException("Capacity");
      reserve((uint)value);
    }
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsSynchronized {
    get {
      return false;
    }
  }

  public void CopyTo(GameObject[] array)
  {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(GameObject[] array, int arrayIndex)
  {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, GameObject[] array, int arrayIndex, int count)
  {
    if (array == null)
      throw new global::System.ArgumentNullException("array");
    if (index < 0)
      throw new global::System.ArgumentOutOfRangeException("index", "Value is less than zero");
    if (arrayIndex < 0)
      throw new global::System.ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (count < 0)
      throw new global::System.ArgumentOutOfRangeException("count", "Value is less than zero");
    if (array.Rank > 1)
      throw new global::System.ArgumentException("Multi dimensional array.", "array");
    if (index+count > this.Count || arrayIndex+count > array.Length)
      throw new global::System.ArgumentException("Number of elements to copy is too large.");
    for (int i=0; i<count; i++)
      array.SetValue(getitemcopy(index+i), arrayIndex+i);
  }

  global::System.Collections.Generic.IEnumerator<GameObject> global::System.Collections.Generic.IEnumerable<GameObject>.GetEnumerator() {
    return new GameObjectVectorEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new GameObjectVectorEnumerator(this);
  }

  public GameObjectVectorEnumerator GetEnumerator() {
    return new GameObjectVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class GameObjectVectorEnumerator : global::System.Collections.IEnumerator
    , global::System.Collections.Generic.IEnumerator<GameObject>
  {
    private GameObjectVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public GameObjectVectorEnumerator(GameObjectVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public GameObject Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (GameObject)currentObject;
      }
    }

    // Type-unsafe IEnumerator.Current
    object global::System.Collections.IEnumerator.Current {
      get {
        return Current;
      }
    }

    public bool MoveNext() {
      int size = collectionRef.Count;
      bool moveOkay = (currentIndex+1 < size) && (size == currentSize);
      if (moveOkay) {
        currentIndex++;
        currentObject = collectionRef[currentIndex];
      } else {
        currentObject = null;
      }
      return moveOkay;
    }

    public void Reset() {
      currentIndex = -1;
      currentObject = null;
      if (collectionRef.Count != currentSize) {
        throw new global::System.InvalidOperationException("Collection modified.");
      }
    }

    public void Dispose() {
        currentIndex = -1;
        currentObject = null;
    }
  }

  public void Clear() {
    SharpMochaPINVOKE.GameObjectVector_Clear(swigCPtr);
  }

  public void Add(GameObject x) {
    SharpMochaPINVOKE.GameObjectVector_Add(swigCPtr, GameObject.getCPtr(x));
  }

  private uint size() {
    uint ret = SharpMochaPINVOKE.GameObjectVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = SharpMochaPINVOKE.GameObjectVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    SharpMochaPINVOKE.GameObjectVector_reserve(swigCPtr, n);
  }

  public GameObjectVector() : this(SharpMochaPINVOKE.new_GameObjectVector__SWIG_0(), true) {
  }

  public GameObjectVector(GameObjectVector other) : this(SharpMochaPINVOKE.new_GameObjectVector__SWIG_1(GameObjectVector.getCPtr(other)), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public GameObjectVector(int capacity) : this(SharpMochaPINVOKE.new_GameObjectVector__SWIG_2(capacity), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  private GameObject getitemcopy(int index) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObjectVector_getitemcopy(swigCPtr, index);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private GameObject getitem(int index) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObjectVector_getitem(swigCPtr, index);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, GameObject val) {
    SharpMochaPINVOKE.GameObjectVector_setitem(swigCPtr, index, GameObject.getCPtr(val));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(GameObjectVector values) {
    SharpMochaPINVOKE.GameObjectVector_AddRange(swigCPtr, GameObjectVector.getCPtr(values));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public GameObjectVector GetRange(int index, int count) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObjectVector_GetRange(swigCPtr, index, count);
    GameObjectVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObjectVector(cPtr, true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, GameObject x) {
    SharpMochaPINVOKE.GameObjectVector_Insert(swigCPtr, index, GameObject.getCPtr(x));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, GameObjectVector values) {
    SharpMochaPINVOKE.GameObjectVector_InsertRange(swigCPtr, index, GameObjectVector.getCPtr(values));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    SharpMochaPINVOKE.GameObjectVector_RemoveAt(swigCPtr, index);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    SharpMochaPINVOKE.GameObjectVector_RemoveRange(swigCPtr, index, count);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public static GameObjectVector Repeat(GameObject value, int count) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObjectVector_Repeat(GameObject.getCPtr(value), count);
    GameObjectVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObjectVector(cPtr, true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    SharpMochaPINVOKE.GameObjectVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    SharpMochaPINVOKE.GameObjectVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, GameObjectVector values) {
    SharpMochaPINVOKE.GameObjectVector_SetRange(swigCPtr, index, GameObjectVector.getCPtr(values));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool Contains(GameObject value) {
    bool ret = SharpMochaPINVOKE.GameObjectVector_Contains(swigCPtr, GameObject.getCPtr(value));
    return ret;
  }

  public int IndexOf(GameObject value) {
    int ret = SharpMochaPINVOKE.GameObjectVector_IndexOf(swigCPtr, GameObject.getCPtr(value));
    return ret;
  }

  public int LastIndexOf(GameObject value) {
    int ret = SharpMochaPINVOKE.GameObjectVector_LastIndexOf(swigCPtr, GameObject.getCPtr(value));
    return ret;
  }

  public bool Remove(GameObject value) {
    bool ret = SharpMochaPINVOKE.GameObjectVector_Remove(swigCPtr, GameObject.getCPtr(value));
    return ret;
  }

}

}