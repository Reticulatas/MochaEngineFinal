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

public partial class AABB : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal AABB(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(AABB obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~AABB() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_AABB(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public AABB() : this(SharpMochaPINVOKE.new_AABB(), true) {
  }

  public MochaVec3 minPoint {
    set {
      SharpMochaPINVOKE.AABB_minPoint_set(swigCPtr, MochaVec3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.AABB_minPoint_get(swigCPtr);
      MochaVec3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec3(cPtr, false);
      return ret;
    } 
  }

  public MochaVec3 maxPoint {
    set {
      SharpMochaPINVOKE.AABB_maxPoint_set(swigCPtr, MochaVec3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.AABB_maxPoint_get(swigCPtr);
      MochaVec3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec3(cPtr, false);
      return ret;
    } 
  }

}

}