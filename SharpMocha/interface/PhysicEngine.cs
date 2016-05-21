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

public partial class PhysicEngine : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal PhysicEngine(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(PhysicEngine obj) {
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

  public GameObject DragObject(MochaVec4 CamPos, MochaVec4 Mouse) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_DragObject(swigCPtr, MochaVec4.getCPtr(CamPos), MochaVec4.getCPtr(Mouse));
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public GameObject DragObject_DoRaycast(MochaVec4 CamPos, MochaVec4 Mouse, GameObject obj, float arg3) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_DragObject_DoRaycast(swigCPtr, MochaVec4.getCPtr(CamPos), MochaVec4.getCPtr(Mouse), GameObject.getCPtr(obj), arg3);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public GameObject RayCasting(MochaVec4 start, MochaVec4 end, Color color) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_RayCasting(swigCPtr, MochaVec4.getCPtr(start), MochaVec4.getCPtr(end), Color.getCPtr(color));
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public GameObject RayCast(MochaVec4 start, MochaVec4 vec, Color color) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_RayCast(swigCPtr, MochaVec4.getCPtr(start), MochaVec4.getCPtr(vec), Color.getCPtr(color));
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public GameObject RayCast3D(MochaVec3 pos, MochaVec3 dir, float maxDistance) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_RayCast3D__SWIG_0(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(dir), maxDistance);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public GameObject RayCast3D(MochaVec3 pos, MochaVec3 dir, CCamera camera, int screen_x, int screen_y, float maxDist) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_RayCast3D__SWIG_1(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(dir), CCamera.getCPtr(camera), screen_x, screen_y, maxDist);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public GameObject RayCast3DWithTag(MochaVec3 pos, MochaVec3 dir, CCamera camera, int screen_x, int screen_y, float maxDist, string tag) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_RayCast3DWithTag__SWIG_0(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(dir), CCamera.getCPtr(camera), screen_x, screen_y, maxDist, tag);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public GameObject RayCast3DWithTag(MochaVec3 pos, MochaVec3 dir, float maxDistance, string tag) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_RayCast3DWithTag__SWIG_1(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(dir), maxDistance, tag);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RayCastResult RayCast3D_Output(MochaVec3 pos, MochaVec3 dir, float maxDistance) {
    RayCastResult ret = new RayCastResult(SharpMochaPINVOKE.PhysicEngine_RayCast3D_Output(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(dir), maxDistance), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RayCastResult RayCast3DWithTag_Output(MochaVec3 pos, MochaVec3 dir, float maxDistance, SWIGTYPE_p_std__string tag) {
    RayCastResult ret = new RayCastResult(SharpMochaPINVOKE.PhysicEngine_RayCast3DWithTag_Output(swigCPtr, MochaVec3.getCPtr(pos), MochaVec3.getCPtr(dir), maxDistance, SWIGTYPE_p_std__string.getCPtr(tag)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool IsActive() {
    bool ret = SharpMochaPINVOKE.PhysicEngine_IsActive__SWIG_0(swigCPtr);
    return ret;
  }

  public void IsActive(bool val) {
    SharpMochaPINVOKE.PhysicEngine_IsActive__SWIG_1(swigCPtr, val);
  }

  public float GetTimeStep() {
    float ret = SharpMochaPINVOKE.PhysicEngine_GetTimeStep(swigCPtr);
    return ret;
  }

  public MochaVec4 GetGravity() {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.PhysicEngine_GetGravity(swigCPtr), false);
    return ret;
  }

  public static PhysicEngine getInstance() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.PhysicEngine_getInstance();
    PhysicEngine ret = (cPtr == global::System.IntPtr.Zero) ? null : new PhysicEngine(cPtr, false);
    return ret;
  }

}

}
