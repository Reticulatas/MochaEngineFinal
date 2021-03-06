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

public partial class VertexVector : global::System.IDisposable, global::System.Collections.IEnumerable
    , global::System.Collections.Generic.IEnumerable<Vertex>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal VertexVector(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(VertexVector obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~VertexVector() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_VertexVector(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public VertexVector(global::System.Collections.ICollection c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (Vertex element in c) {
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

  public Vertex this[int index]  {
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

  public void CopyTo(Vertex[] array)
  {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(Vertex[] array, int arrayIndex)
  {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, Vertex[] array, int arrayIndex, int count)
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

  global::System.Collections.Generic.IEnumerator<Vertex> global::System.Collections.Generic.IEnumerable<Vertex>.GetEnumerator() {
    return new VertexVectorEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new VertexVectorEnumerator(this);
  }

  public VertexVectorEnumerator GetEnumerator() {
    return new VertexVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class VertexVectorEnumerator : global::System.Collections.IEnumerator
    , global::System.Collections.Generic.IEnumerator<Vertex>
  {
    private VertexVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public VertexVectorEnumerator(VertexVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public Vertex Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (Vertex)currentObject;
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
    SharpMochaPINVOKE.VertexVector_Clear(swigCPtr);
  }

  public void Add(Vertex x) {
    SharpMochaPINVOKE.VertexVector_Add(swigCPtr, Vertex.getCPtr(x));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  private uint size() {
    uint ret = SharpMochaPINVOKE.VertexVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = SharpMochaPINVOKE.VertexVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    SharpMochaPINVOKE.VertexVector_reserve(swigCPtr, n);
  }

  public VertexVector() : this(SharpMochaPINVOKE.new_VertexVector__SWIG_0(), true) {
  }

  public VertexVector(VertexVector other) : this(SharpMochaPINVOKE.new_VertexVector__SWIG_1(VertexVector.getCPtr(other)), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public VertexVector(int capacity) : this(SharpMochaPINVOKE.new_VertexVector__SWIG_2(capacity), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  private Vertex getitemcopy(int index) {
    Vertex ret = new Vertex(SharpMochaPINVOKE.VertexVector_getitemcopy(swigCPtr, index), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private Vertex getitem(int index) {
    Vertex ret = new Vertex(SharpMochaPINVOKE.VertexVector_getitem(swigCPtr, index), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, Vertex val) {
    SharpMochaPINVOKE.VertexVector_setitem(swigCPtr, index, Vertex.getCPtr(val));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(VertexVector values) {
    SharpMochaPINVOKE.VertexVector_AddRange(swigCPtr, VertexVector.getCPtr(values));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public VertexVector GetRange(int index, int count) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.VertexVector_GetRange(swigCPtr, index, count);
    VertexVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new VertexVector(cPtr, true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, Vertex x) {
    SharpMochaPINVOKE.VertexVector_Insert(swigCPtr, index, Vertex.getCPtr(x));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, VertexVector values) {
    SharpMochaPINVOKE.VertexVector_InsertRange(swigCPtr, index, VertexVector.getCPtr(values));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    SharpMochaPINVOKE.VertexVector_RemoveAt(swigCPtr, index);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    SharpMochaPINVOKE.VertexVector_RemoveRange(swigCPtr, index, count);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public static VertexVector Repeat(Vertex value, int count) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.VertexVector_Repeat(Vertex.getCPtr(value), count);
    VertexVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new VertexVector(cPtr, true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    SharpMochaPINVOKE.VertexVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    SharpMochaPINVOKE.VertexVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, VertexVector values) {
    SharpMochaPINVOKE.VertexVector_SetRange(swigCPtr, index, VertexVector.getCPtr(values));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
