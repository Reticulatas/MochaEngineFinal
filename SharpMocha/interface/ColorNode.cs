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

public partial class ColorNode : MatAttribNode {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal ColorNode(global::System.IntPtr cPtr, bool cMemoryOwn) : base(SharpMochaPINVOKE.ColorNode_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ColorNode obj) {
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

  public Color color {
    set {
      SharpMochaPINVOKE.ColorNode_color_set(swigCPtr, Color.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.ColorNode_color_get(swigCPtr);
      Color ret = (cPtr == global::System.IntPtr.Zero) ? null : new Color(cPtr, false);
      return ret;
    } 
  }

  public MochaVec2 mTextureOffset {
    set {
      SharpMochaPINVOKE.ColorNode_mTextureOffset_set(swigCPtr, MochaVec2.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.ColorNode_mTextureOffset_get(swigCPtr);
      MochaVec2 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec2(cPtr, false);
      return ret;
    } 
  }

  public MochaVec2 mTextureScale {
    set {
      SharpMochaPINVOKE.ColorNode_mTextureScale_set(swigCPtr, MochaVec2.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.ColorNode_mTextureScale_get(swigCPtr);
      MochaVec2 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec2(cPtr, false);
      return ret;
    } 
  }

  public virtual void BindAttribute(SWIGTYPE_p_Shader shader) {
    SharpMochaPINVOKE.ColorNode_BindAttribute(swigCPtr, SWIGTYPE_p_Shader.getCPtr(shader));
  }

  public void SetColor(Color col) {
    SharpMochaPINVOKE.ColorNode_SetColor(swigCPtr, Color.getCPtr(col));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
