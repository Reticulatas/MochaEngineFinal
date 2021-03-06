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

public partial class RigidBodyComponent : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal RigidBodyComponent(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(RigidBodyComponent obj) {
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

  public RigidBody Body() {
    RigidBody ret = new RigidBody(SharpMochaPINVOKE.RigidBodyComponent_Body(swigCPtr), false);
    return ret;
  }

  public int GetType() {
    int ret = SharpMochaPINVOKE.RigidBodyComponent_GetType(swigCPtr);
    return ret;
  }

  public RigidBodyComponent SetType(int type) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetType(swigCPtr, type), false);
    return ret;
  }

  public bool GetAwake() {
    bool ret = SharpMochaPINVOKE.RigidBodyComponent_GetAwake(swigCPtr);
    return ret;
  }

  public RigidBodyComponent SetAwake(bool awake) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetAwake(swigCPtr, awake), false);
    return ret;
  }

  public MochaVec3 GetCentroid() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.RigidBodyComponent_GetCentroid(swigCPtr), false);
    return ret;
  }

  public MochaVec3 GetPosition() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.RigidBodyComponent_GetPosition(swigCPtr), false);
    return ret;
  }

  public RigidBodyComponent SetPosition(MochaVec3 pos) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetPosition(swigCPtr, MochaVec3.getCPtr(pos)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public SWIGTYPE_p_Math__Matrix3 GetOrientation() {
    SWIGTYPE_p_Math__Matrix3 ret = new SWIGTYPE_p_Math__Matrix3(SharpMochaPINVOKE.RigidBodyComponent_GetOrientation(swigCPtr), false);
    return ret;
  }

  public RigidBodyComponent SetOrientation(SWIGTYPE_p_Math__Matrix3 orientation) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetOrientation(swigCPtr, SWIGTYPE_p_Math__Matrix3.getCPtr(orientation)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec3 Forward() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.RigidBodyComponent_Forward(swigCPtr), true);
    return ret;
  }

  public MochaVec3 Right() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.RigidBodyComponent_Right(swigCPtr), true);
    return ret;
  }

  public MochaVec3 Up() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.RigidBodyComponent_Up(swigCPtr), true);
    return ret;
  }

  public MochaVec3 GetLinearVelocity() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.RigidBodyComponent_GetLinearVelocity(swigCPtr), true);
    return ret;
  }

  public RigidBodyComponent SetLinearVelocity(MochaVec3 pos) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetLinearVelocity(swigCPtr, MochaVec3.getCPtr(pos)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec3 GetAngularVelocity() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.RigidBodyComponent_GetAngularVelocity(swigCPtr), true);
    return ret;
  }

  public RigidBodyComponent SetAngularVelocity(MochaVec3 pos) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetAngularVelocity(swigCPtr, MochaVec3.getCPtr(pos)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyForce(MochaVec3 force, MochaVec3 at) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyForce(swigCPtr, MochaVec3.getCPtr(force), MochaVec3.getCPtr(at)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyLinearForce(MochaVec3 force) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyLinearForce(swigCPtr, MochaVec3.getCPtr(force)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyAngularForce(MochaVec3 force, MochaVec3 at) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyAngularForce(swigCPtr, MochaVec3.getCPtr(force), MochaVec3.getCPtr(at)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyMasslessForce(MochaVec3 force, MochaVec3 at) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyMasslessForce(swigCPtr, MochaVec3.getCPtr(force), MochaVec3.getCPtr(at)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyLinearMasslessForce(MochaVec3 force) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyLinearMasslessForce(swigCPtr, MochaVec3.getCPtr(force)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyAngularMasslessForce(MochaVec3 force, MochaVec3 at) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyAngularMasslessForce(swigCPtr, MochaVec3.getCPtr(force), MochaVec3.getCPtr(at)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyImpulse(MochaVec3 impulse, MochaVec3 at) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyImpulse(swigCPtr, MochaVec3.getCPtr(impulse), MochaVec3.getCPtr(at)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyLinearImpulse(MochaVec3 impulse) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyLinearImpulse(swigCPtr, MochaVec3.getCPtr(impulse)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyAngularImpulse(MochaVec3 impulse, MochaVec3 at) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyAngularImpulse(swigCPtr, MochaVec3.getCPtr(impulse), MochaVec3.getCPtr(at)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public RigidBodyComponent ApplyImmediateImpulse(MochaVec3 impulse, MochaVec3 at) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_ApplyImmediateImpulse(swigCPtr, MochaVec3.getCPtr(impulse), MochaVec3.getCPtr(at)), false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool GetAllowRotation() {
    bool ret = SharpMochaPINVOKE.RigidBodyComponent_GetAllowRotation(swigCPtr);
    return ret;
  }

  public RigidBodyComponent SetAllowRotation(bool allowRotation) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetAllowRotation(swigCPtr, allowRotation), false);
    return ret;
  }

  public bool GetHasGravity() {
    bool ret = SharpMochaPINVOKE.RigidBodyComponent_GetHasGravity(swigCPtr);
    return ret;
  }

  public RigidBodyComponent SetHasGravity(bool hasGravity) {
    RigidBodyComponent ret = new RigidBodyComponent(SharpMochaPINVOKE.RigidBodyComponent_SetHasGravity(swigCPtr, hasGravity), false);
    return ret;
  }

}

}
