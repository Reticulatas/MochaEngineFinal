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

public partial class Prefab : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Prefab(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Prefab obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          throw new global::System.MethodAccessException("C++ destructor does not have public access");
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public static Prefab MakePrefabFrom(GameObject arg0) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.Prefab_MakePrefabFrom(GameObject.getCPtr(arg0));
    Prefab ret = (cPtr == global::System.IntPtr.Zero) ? null : new Prefab(cPtr, false);
    return ret;
  }

  public string name {
    set {
      SharpMochaPINVOKE.Prefab_name_set(swigCPtr, value);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      string ret = SharpMochaPINVOKE.Prefab_name_get(swigCPtr);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public string filename {
    set {
      SharpMochaPINVOKE.Prefab_filename_set(swigCPtr, value);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      string ret = SharpMochaPINVOKE.Prefab_filename_get(swigCPtr);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public bool isTransient {
    set {
      SharpMochaPINVOKE.Prefab_isTransient_set(swigCPtr, value);
    } 
    get {
      bool ret = SharpMochaPINVOKE.Prefab_isTransient_get(swigCPtr);
      return ret;
    } 
  }

  public string stringBuf {
    set {
      SharpMochaPINVOKE.Prefab_stringBuf_set(swigCPtr, value);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      string ret = SharpMochaPINVOKE.Prefab_stringBuf_get(swigCPtr);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public GameObject dormantObject {
    set {
      SharpMochaPINVOKE.Prefab_dormantObject_set(swigCPtr, GameObject.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.Prefab_dormantObject_get(swigCPtr);
      GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
      return ret;
    } 
  }

  public void CreateInstance(GameObject gObj) {
    SharpMochaPINVOKE.Prefab_CreateInstance(swigCPtr, GameObject.getCPtr(gObj));
  }

  public void Rename(SWIGTYPE_p_std__string newName) {
    SharpMochaPINVOKE.Prefab_Rename(swigCPtr, SWIGTYPE_p_std__string.getCPtr(newName));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
