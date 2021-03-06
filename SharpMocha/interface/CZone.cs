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

public partial class CZone : Component {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal CZone(global::System.IntPtr cPtr, bool cMemoryOwn) : base(SharpMochaPINVOKE.CZone_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(CZone obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          throw new global::System.MethodAccessException("C++ destructor does not have public access");
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public MochaVec3 GetCenter() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CZone_GetCenter(swigCPtr), false);
    return ret;
  }

  public float width {
    set {
      SharpMochaPINVOKE.CZone_width_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.CZone_width_get(swigCPtr);
      return ret;
    } 
  }

  public float height {
    set {
      SharpMochaPINVOKE.CZone_height_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.CZone_height_get(swigCPtr);
      return ret;
    } 
  }

  public float depth {
    set {
      SharpMochaPINVOKE.CZone_depth_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.CZone_depth_get(swigCPtr);
      return ret;
    } 
  }

  public float GetLeftBound() {
    float ret = SharpMochaPINVOKE.CZone_GetLeftBound(swigCPtr);
    return ret;
  }

  public float GetRightBound() {
    float ret = SharpMochaPINVOKE.CZone_GetRightBound(swigCPtr);
    return ret;
  }

  public float GetBottomBound() {
    float ret = SharpMochaPINVOKE.CZone_GetBottomBound(swigCPtr);
    return ret;
  }

  public float GetTopBound() {
    float ret = SharpMochaPINVOKE.CZone_GetTopBound(swigCPtr);
    return ret;
  }

  public float GetFrontBound() {
    float ret = SharpMochaPINVOKE.CZone_GetFrontBound(swigCPtr);
    return ret;
  }

  public float GetBackBound() {
    float ret = SharpMochaPINVOKE.CZone_GetBackBound(swigCPtr);
    return ret;
  }

  public MochaVec3 GetDiagonalCornerZero() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CZone_GetDiagonalCornerZero(swigCPtr), true);
    return ret;
  }

  public MochaVec3 GetDiagonalCornerOne() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CZone_GetDiagonalCornerOne(swigCPtr), true);
    return ret;
  }

  public bool IsPointWithin(MochaVec3 point) {
    bool ret = SharpMochaPINVOKE.CZone_IsPointWithin(swigCPtr, MochaVec3.getCPtr(point));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void ClampObjectToBounds(GameObject gobj) {
    SharpMochaPINVOKE.CZone_ClampObjectToBounds(swigCPtr, GameObject.getCPtr(gobj));
  }

}

}
