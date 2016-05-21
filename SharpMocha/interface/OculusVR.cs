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

public partial class OculusVR : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal OculusVR(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(OculusVR obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~OculusVR() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_OculusVR(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public bool IsConnected() {
    bool ret = SharpMochaPINVOKE.OculusVR_IsConnected(swigCPtr);
    return ret;
  }

  public uint GetRecommendedWidth() {
    uint ret = SharpMochaPINVOKE.OculusVR_GetRecommendedWidth(swigCPtr);
    return ret;
  }

  public uint GetRecommendedHeight() {
    uint ret = SharpMochaPINVOKE.OculusVR_GetRecommendedHeight(swigCPtr);
    return ret;
  }

  public static OculusVR getInstance() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.OculusVR_getInstance();
    OculusVR ret = (cPtr == global::System.IntPtr.Zero) ? null : new OculusVR(cPtr, false);
    return ret;
  }

  public MochaVec3 GetEyePos(uint index) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.OculusVR_GetEyePos(swigCPtr, index), true);
    return ret;
  }

  public OculusVR() : this(SharpMochaPINVOKE.new_OculusVR(), true) {
  }

}

}