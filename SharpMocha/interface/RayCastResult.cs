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

public partial class RayCastResult : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal RayCastResult(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(RayCastResult obj) {
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

  public bool hit {
    set {
      SharpMochaPINVOKE.RayCastResult_hit_set(swigCPtr, value);
    } 
    get {
      bool ret = SharpMochaPINVOKE.RayCastResult_hit_get(swigCPtr);
      return ret;
    } 
  }

  public float t {
    set {
      SharpMochaPINVOKE.RayCastResult_t_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.RayCastResult_t_get(swigCPtr);
      return ret;
    } 
  }

  public MochaVec3 intersection {
    set {
      SharpMochaPINVOKE.RayCastResult_intersection_set(swigCPtr, MochaVec3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.RayCastResult_intersection_get(swigCPtr);
      MochaVec3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec3(cPtr, false);
      return ret;
    } 
  }

  public MochaVec3 normal {
    set {
      SharpMochaPINVOKE.RayCastResult_normal_set(swigCPtr, MochaVec3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.RayCastResult_normal_get(swigCPtr);
      MochaVec3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec3(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_Collider collider {
    set {
      SharpMochaPINVOKE.RayCastResult_collider_set(swigCPtr, SWIGTYPE_p_Collider.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.RayCastResult_collider_get(swigCPtr);
      SWIGTYPE_p_Collider ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_Collider(cPtr, false);
      return ret;
    } 
  }

  public GameObject go {
    set {
      SharpMochaPINVOKE.RayCastResult_go_set(swigCPtr, GameObject.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.RayCastResult_go_get(swigCPtr);
      GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
      return ret;
    } 
  }

}

}