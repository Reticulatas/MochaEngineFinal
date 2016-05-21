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

public partial class IZone : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal IZone(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(IZone obj) {
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

  public float width {
    set {
      SharpMochaPINVOKE.IZone_width_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.IZone_width_get(swigCPtr);
      return ret;
    } 
  }

  public float height {
    set {
      SharpMochaPINVOKE.IZone_height_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.IZone_height_get(swigCPtr);
      return ret;
    } 
  }

  public float depth {
    set {
      SharpMochaPINVOKE.IZone_depth_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.IZone_depth_get(swigCPtr);
      return ret;
    } 
  }

  public float GetLeftBound() {
    float ret = SharpMochaPINVOKE.IZone_GetLeftBound(swigCPtr);
    return ret;
  }

  public float GetRightBound() {
    float ret = SharpMochaPINVOKE.IZone_GetRightBound(swigCPtr);
    return ret;
  }

  public float GetBottomBound() {
    float ret = SharpMochaPINVOKE.IZone_GetBottomBound(swigCPtr);
    return ret;
  }

  public float GetTopBound() {
    float ret = SharpMochaPINVOKE.IZone_GetTopBound(swigCPtr);
    return ret;
  }

  public float GetFrontBound() {
    float ret = SharpMochaPINVOKE.IZone_GetFrontBound(swigCPtr);
    return ret;
  }

  public float GetBackBound() {
    float ret = SharpMochaPINVOKE.IZone_GetBackBound(swigCPtr);
    return ret;
  }

  public void Init(GameObject g) {
    SharpMochaPINVOKE.IZone_Init(swigCPtr, GameObject.getCPtr(g));
  }

  public MochaVec3 GetDiagonalCornerZero() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.IZone_GetDiagonalCornerZero(swigCPtr), true);
    return ret;
  }

  public MochaVec3 GetDiagonalCornerOne() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.IZone_GetDiagonalCornerOne(swigCPtr), true);
    return ret;
  }

  public bool IsPointWithin(MochaVec3 point) {
    bool ret = SharpMochaPINVOKE.IZone_IsPointWithin(swigCPtr, MochaVec3.getCPtr(point));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void ClampObjectToBounds(GameObject gobj) {
    SharpMochaPINVOKE.IZone_ClampObjectToBounds(swigCPtr, GameObject.getCPtr(gobj));
  }

  public GameObjectVector GetObjectsInBounds() {
    GameObjectVector ret = new GameObjectVector(SharpMochaPINVOKE.IZone_GetObjectsInBounds(swigCPtr), true);
    return ret;
  }

  public void DrawBounds() {
    SharpMochaPINVOKE.IZone_DrawBounds(swigCPtr);
  }

}

}