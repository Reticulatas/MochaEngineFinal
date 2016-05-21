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

public partial class Renderer : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Renderer(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Renderer obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Renderer() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_Renderer(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public MochaVec4 mGlobalAmbient {
    set {
      SharpMochaPINVOKE.Renderer_mGlobalAmbient_set(swigCPtr, MochaVec4.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.Renderer_mGlobalAmbient_get(swigCPtr);
      MochaVec4 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec4(cPtr, false);
      return ret;
    } 
  }

  public static Renderer getInstance() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.Renderer_getInstance();
    Renderer ret = (cPtr == global::System.IntPtr.Zero) ? null : new Renderer(cPtr, false);
    return ret;
  }

  public void DrawBoxPerma(MochaVec3 pos, MochaVec3 halflengths, Color col) {
    SharpMochaPINVOKE.Renderer_DrawBoxPerma__SWIG_0(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(halflengths), Color.getCPtr(col));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void DrawBoxPerma(MochaVec3 pos, MochaVec3 halflengths) {
    SharpMochaPINVOKE.Renderer_DrawBoxPerma__SWIG_1(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(halflengths));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void DrawLine(MochaVec3 begin, MochaVec3 end, Color col) {
    SharpMochaPINVOKE.Renderer_DrawLine(swigCPtr, MochaVec3.getCPtr(begin), MochaVec3.getCPtr(end), Color.getCPtr(col));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void DrawLinePerma(MochaVec3 begin, MochaVec3 end, Color col) {
    SharpMochaPINVOKE.Renderer_DrawLinePerma(swigCPtr, MochaVec3.getCPtr(begin), MochaVec3.getCPtr(end), Color.getCPtr(col));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public Renderer() : this(SharpMochaPINVOKE.new_Renderer(), true) {
  }

}

}