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

public partial class CMapZone : Component {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal CMapZone(global::System.IntPtr cPtr, bool cMemoryOwn) : base(SharpMochaPINVOKE.CMapZone_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(CMapZone obj) {
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

  public string id {
    set {
      SharpMochaPINVOKE.CMapZone_id_set(swigCPtr, value);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      string ret = SharpMochaPINVOKE.CMapZone_id_get(swigCPtr);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public IZone zone {
    set {
      SharpMochaPINVOKE.CMapZone_zone_set(swigCPtr, IZone.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.CMapZone_zone_get(swigCPtr);
      IZone ret = (cPtr == global::System.IntPtr.Zero) ? null : new IZone(cPtr, false);
      return ret;
    } 
  }

}

}