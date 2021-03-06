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

public class SkinnedVertex : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal SkinnedVertex(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(SkinnedVertex obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~SkinnedVertex() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_SkinnedVertex(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public SkinnedVertex() : this(SharpMochaPINVOKE.new_SkinnedVertex(), true) {
  }

  public SWIGTYPE_p_Vector3 mPosition {
    set {
      SharpMochaPINVOKE.SkinnedVertex_mPosition_set(swigCPtr, SWIGTYPE_p_Vector3.getCPtr(value));
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_Vector3 ret = new SWIGTYPE_p_Vector3(SharpMochaPINVOKE.SkinnedVertex_mPosition_get(swigCPtr), true);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public Color mColor {
    set {
      SharpMochaPINVOKE.SkinnedVertex_mColor_set(swigCPtr, Color.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.SkinnedVertex_mColor_get(swigCPtr);
      Color ret = (cPtr == global::System.IntPtr.Zero) ? null : new Color(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_Vector2 mTextureCoords {
    set {
      SharpMochaPINVOKE.SkinnedVertex_mTextureCoords_set(swigCPtr, SWIGTYPE_p_Vector2.getCPtr(value));
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_Vector2 ret = new SWIGTYPE_p_Vector2(SharpMochaPINVOKE.SkinnedVertex_mTextureCoords_get(swigCPtr), true);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public SWIGTYPE_p_Vector3 mNormal {
    set {
      SharpMochaPINVOKE.SkinnedVertex_mNormal_set(swigCPtr, SWIGTYPE_p_Vector3.getCPtr(value));
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_Vector3 ret = new SWIGTYPE_p_Vector3(SharpMochaPINVOKE.SkinnedVertex_mNormal_get(swigCPtr), true);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public SWIGTYPE_p_Vector3 mTangent {
    set {
      SharpMochaPINVOKE.SkinnedVertex_mTangent_set(swigCPtr, SWIGTYPE_p_Vector3.getCPtr(value));
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_Vector3 ret = new SWIGTYPE_p_Vector3(SharpMochaPINVOKE.SkinnedVertex_mTangent_get(swigCPtr), true);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public SWIGTYPE_p_Vector3 mBinormal {
    set {
      SharpMochaPINVOKE.SkinnedVertex_mBinormal_set(swigCPtr, SWIGTYPE_p_Vector3.getCPtr(value));
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_Vector3 ret = new SWIGTYPE_p_Vector3(SharpMochaPINVOKE.SkinnedVertex_mBinormal_get(swigCPtr), true);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public SWIGTYPE_p_Vector4 mBlendWeights {
    set {
      SharpMochaPINVOKE.SkinnedVertex_mBlendWeights_set(swigCPtr, SWIGTYPE_p_Vector4.getCPtr(value));
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_Vector4 ret = new SWIGTYPE_p_Vector4(SharpMochaPINVOKE.SkinnedVertex_mBlendWeights_get(swigCPtr), true);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

}

}
