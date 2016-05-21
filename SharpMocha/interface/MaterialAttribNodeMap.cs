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

public partial class MaterialAttribNodeMap : global::System.IDisposable 
    , global::System.Collections.Generic.IDictionary<uint, MaterialAttribNodeVector>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal MaterialAttribNodeMap(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(MaterialAttribNodeMap obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~MaterialAttribNodeMap() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_MaterialAttribNodeMap(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }


  public MaterialAttribNodeVector this[uint key] {
    get {
      return getitem(key);
    }

    set {
      setitem(key, value);
    }
  }

  public bool TryGetValue(uint key, out MaterialAttribNodeVector value) {
    if (this.ContainsKey(key)) {
      value = this[key];
      return true;
    }
    value = default(MaterialAttribNodeVector);
    return false;
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsReadOnly {
    get { 
      return false; 
    }
  }

  public global::System.Collections.Generic.ICollection<uint> Keys {
    get {
      global::System.Collections.Generic.ICollection<uint> keys = new global::System.Collections.Generic.List<uint>();
      int size = this.Count;
      if (size > 0) {
        global::System.IntPtr iter = create_iterator_begin();
        for (int i = 0; i < size; i++) {
          keys.Add(get_next_key(iter));
        }
        destroy_iterator(iter);
      }
      return keys;
    }
  }

  public global::System.Collections.Generic.ICollection<MaterialAttribNodeVector> Values {
    get {
      global::System.Collections.Generic.ICollection<MaterialAttribNodeVector> vals = new global::System.Collections.Generic.List<MaterialAttribNodeVector>();
      foreach (global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector> pair in this) {
        vals.Add(pair.Value);
      }
      return vals;
    }
  }
  
  public void Add(global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector> item) {
    Add(item.Key, item.Value);
  }

  public bool Remove(global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector> item) {
    if (Contains(item)) {
      return Remove(item.Key);
    } else {
      return false;
    }
  }

  public bool Contains(global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector> item) {
    if (this[item.Key] == item.Value) {
      return true;
    } else {
      return false;
    }
  }

  public void CopyTo(global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>[] array) {
    CopyTo(array, 0);
  }

  public void CopyTo(global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>[] array, int arrayIndex) {
    if (array == null)
      throw new global::System.ArgumentNullException("array");
    if (arrayIndex < 0)
      throw new global::System.ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (array.Rank > 1)
      throw new global::System.ArgumentException("Multi dimensional array.", "array");
    if (arrayIndex+this.Count > array.Length)
      throw new global::System.ArgumentException("Number of elements to copy is too large.");

    global::System.Collections.Generic.IList<uint> keyList = new global::System.Collections.Generic.List<uint>(this.Keys);
    for (int i = 0; i < keyList.Count; i++) {
      uint currentKey = keyList[i];
      array.SetValue(new global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>(currentKey, this[currentKey]), arrayIndex+i);
    }
  }

  global::System.Collections.Generic.IEnumerator<global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>> global::System.Collections.Generic.IEnumerable<global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>>.GetEnumerator() {
    return new MaterialAttribNodeMapEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new MaterialAttribNodeMapEnumerator(this);
  }

  public MaterialAttribNodeMapEnumerator GetEnumerator() {
    return new MaterialAttribNodeMapEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class MaterialAttribNodeMapEnumerator : global::System.Collections.IEnumerator, 
      global::System.Collections.Generic.IEnumerator<global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>>
  {
    private MaterialAttribNodeMap collectionRef;
    private global::System.Collections.Generic.IList<uint> keyCollection;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public MaterialAttribNodeMapEnumerator(MaterialAttribNodeMap collection) {
      collectionRef = collection;
      keyCollection = new global::System.Collections.Generic.List<uint>(collection.Keys);
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector> Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>)currentObject;
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
        uint currentKey = keyCollection[currentIndex];
        currentObject = new global::System.Collections.Generic.KeyValuePair<uint, MaterialAttribNodeVector>(currentKey, collectionRef[currentKey]);
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
  

  public MaterialAttribNodeMap() : this(SharpMochaPINVOKE.new_MaterialAttribNodeMap__SWIG_0(), true) {
  }

  public MaterialAttribNodeMap(MaterialAttribNodeMap other) : this(SharpMochaPINVOKE.new_MaterialAttribNodeMap__SWIG_1(MaterialAttribNodeMap.getCPtr(other)), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  private uint size() {
    uint ret = SharpMochaPINVOKE.MaterialAttribNodeMap_size(swigCPtr);
    return ret;
  }

  public bool empty() {
    bool ret = SharpMochaPINVOKE.MaterialAttribNodeMap_empty(swigCPtr);
    return ret;
  }

  public void Clear() {
    SharpMochaPINVOKE.MaterialAttribNodeMap_Clear(swigCPtr);
  }

  private MaterialAttribNodeVector getitem(uint key) {
    MaterialAttribNodeVector ret = new MaterialAttribNodeVector(SharpMochaPINVOKE.MaterialAttribNodeMap_getitem(swigCPtr, key), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(uint key, MaterialAttribNodeVector x) {
    SharpMochaPINVOKE.MaterialAttribNodeMap_setitem(swigCPtr, key, MaterialAttribNodeVector.getCPtr(x));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool ContainsKey(uint key) {
    bool ret = SharpMochaPINVOKE.MaterialAttribNodeMap_ContainsKey(swigCPtr, key);
    return ret;
  }

  public void Add(uint key, MaterialAttribNodeVector val) {
    SharpMochaPINVOKE.MaterialAttribNodeMap_Add(swigCPtr, key, MaterialAttribNodeVector.getCPtr(val));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool Remove(uint key) {
    bool ret = SharpMochaPINVOKE.MaterialAttribNodeMap_Remove(swigCPtr, key);
    return ret;
  }

  private global::System.IntPtr create_iterator_begin() {
    global::System.IntPtr ret = SharpMochaPINVOKE.MaterialAttribNodeMap_create_iterator_begin(swigCPtr);
    return ret;
  }

  private uint get_next_key(global::System.IntPtr swigiterator) {
    uint ret = SharpMochaPINVOKE.MaterialAttribNodeMap_get_next_key(swigCPtr, swigiterator);
    return ret;
  }

  private void destroy_iterator(global::System.IntPtr swigiterator) {
    SharpMochaPINVOKE.MaterialAttribNodeMap_destroy_iterator(swigCPtr, swigiterator);
  }

}

}