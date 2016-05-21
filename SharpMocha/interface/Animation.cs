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

public partial class Animation : Asset {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal Animation(global::System.IntPtr cPtr, bool cMemoryOwn) : base(SharpMochaPINVOKE.Animation_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Animation obj) {
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

  public Animation(string name) : this(SharpMochaPINVOKE.new_Animation__SWIG_0(name), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public Animation() : this(SharpMochaPINVOKE.new_Animation__SWIG_1(), true) {
  }

  public virtual void OnAssetFileChanged() {
    SharpMochaPINVOKE.Animation_OnAssetFileChanged(swigCPtr);
  }

  public float GetDuration() {
    float ret = SharpMochaPINVOKE.Animation_GetDuration(swigCPtr);
    return ret;
  }

  public SWIGTYPE_p_std__vectorT_Track_t GetTracks() {
    SWIGTYPE_p_std__vectorT_Track_t ret = new SWIGTYPE_p_std__vectorT_Track_t(SharpMochaPINVOKE.Animation_GetTracks(swigCPtr), false);
    return ret;
  }

  public new string GetName() {
    string ret = SharpMochaPINVOKE.Animation_GetName(swigCPtr);
    return ret;
  }

}

}
