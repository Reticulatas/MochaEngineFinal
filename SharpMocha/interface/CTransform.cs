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

public partial class CTransform : Component {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal CTransform(global::System.IntPtr cPtr, bool cMemoryOwn) : base(SharpMochaPINVOKE.CTransform_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(CTransform obj) {
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

	[MochaDoNotShow]
  public MochaVec3 position {
    get {
		return GetPosition(TransformSpace.GLOBAL);
    }
  }

  public MochaVec3 localPosition {
    set {
      SharpMochaPINVOKE.CTransform_localPosition_set(swigCPtr, MochaVec3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.CTransform_localPosition_get(swigCPtr);
      MochaVec3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec3(cPtr, false);
      return ret;
    } 
  }

  public MochaVec3 scale {
    set {
      SharpMochaPINVOKE.CTransform_scale_set(swigCPtr, MochaVec3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.CTransform_scale_get(swigCPtr);
      MochaVec3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaVec3(cPtr, false);
      return ret;
    } 
  }

  [MochaHint("RadToDegConverter")]
  public EulerAngles rotation {
    set {
      SharpMochaPINVOKE.CTransform_rotation_set(swigCPtr, EulerAngles.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.CTransform_rotation_get(swigCPtr);
      EulerAngles ret = (cPtr == global::System.IntPtr.Zero) ? null : new EulerAngles(cPtr, false);
      return ret;
    } 
  }

  public MochaVec3 GetForwardVector() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetForwardVector(swigCPtr), true);
    return ret;
  }

  public MochaVec3 GetUpVector() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetUpVector(swigCPtr), true);
    return ret;
  }

  public MochaVec3 GetRightVector() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetRightVector(swigCPtr), true);
    return ret;
  }

  public MochaVec3 GetGlobalPosition() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetGlobalPosition(swigCPtr), true);
    return ret;
  }

  public void LookAt(MochaVec3 point, MochaVec3 worldUp) {
    SharpMochaPINVOKE.CTransform_LookAt__SWIG_0(swigCPtr, MochaVec3.getCPtr(point), MochaVec3.getCPtr(worldUp));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void LookAt(MochaVec3 point) {
    SharpMochaPINVOKE.CTransform_LookAt__SWIG_1(swigCPtr, MochaVec3.getCPtr(point));
  }

  public void Rotate(float x, float y, float z, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_Rotate__SWIG_0(swigCPtr, x, y, z, (int)space);
  }

  public void Rotate(float x, float y, float z) {
    SharpMochaPINVOKE.CTransform_Rotate__SWIG_1(swigCPtr, x, y, z);
  }

  public void Rotate(MochaVec3 vec, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_Rotate__SWIG_2(swigCPtr, MochaVec3.getCPtr(vec), (int)space);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void Rotate(MochaVec3 vec) {
    SharpMochaPINVOKE.CTransform_Rotate__SWIG_3(swigCPtr, MochaVec3.getCPtr(vec));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRotation(MochaVec3 rot, float angle, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetRotation__SWIG_0(swigCPtr, MochaVec3.getCPtr(rot), angle, (int)space);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRotation(MochaVec3 rot, float angle) {
    SharpMochaPINVOKE.CTransform_SetRotation__SWIG_1(swigCPtr, MochaVec3.getCPtr(rot), angle);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRotation(float x, float y, float z, float angle, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetRotation__SWIG_2(swigCPtr, x, y, z, angle, (int)space);
  }

  public void SetRotation(float x, float y, float z, float angle) {
    SharpMochaPINVOKE.CTransform_SetRotation__SWIG_3(swigCPtr, x, y, z, angle);
  }

  public MochaVec3 GetConstrainedRotation() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetConstrainedRotation(swigCPtr), true);
    return ret;
  }

  public MochaVec3 GetRotation(CTransform.TransformSpace space) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetRotation__SWIG_0(swigCPtr, (int)space), true);
    return ret;
  }

  public MochaVec3 GetRotation() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetRotation__SWIG_1(swigCPtr), true);
    return ret;
  }

  public void RotateAround(float x, float y, float z, float angle, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_RotateAround__SWIG_0(swigCPtr, x, y, z, angle, (int)space);
  }

  public void RotateAround(float x, float y, float z, float angle) {
    SharpMochaPINVOKE.CTransform_RotateAround__SWIG_1(swigCPtr, x, y, z, angle);
  }

  public void RotateAround(MochaVec3 vec, float angle, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_RotateAround__SWIG_2(swigCPtr, MochaVec3.getCPtr(vec), angle, (int)space);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void RotateAround(MochaVec3 vec, float angle) {
    SharpMochaPINVOKE.CTransform_RotateAround__SWIG_3(swigCPtr, MochaVec3.getCPtr(vec), angle);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public MochaVec3 GetPosition(CTransform.TransformSpace space) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetPosition(swigCPtr, (int)space), true);
    return ret;
  }

  public void SetPosition(MochaVec3 pos, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetPosition__SWIG_0(swigCPtr, MochaVec3.getCPtr(pos), (int)space);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetPosition(MochaVec3 pos) {
    SharpMochaPINVOKE.CTransform_SetPosition__SWIG_1(swigCPtr, MochaVec3.getCPtr(pos));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetPosition(float x, float y, float z, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetPosition__SWIG_2(swigCPtr, x, y, z, (int)space);
  }

  public void SetPosition(float x, float y, float z) {
    SharpMochaPINVOKE.CTransform_SetPosition__SWIG_3(swigCPtr, x, y, z);
  }

  public void SetPositionX(float x, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetPositionX__SWIG_0(swigCPtr, x, (int)space);
  }

  public void SetPositionX(float x) {
    SharpMochaPINVOKE.CTransform_SetPositionX__SWIG_1(swigCPtr, x);
  }

  public void SetPositionY(float y, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetPositionY__SWIG_0(swigCPtr, y, (int)space);
  }

  public void SetPositionY(float y) {
    SharpMochaPINVOKE.CTransform_SetPositionY__SWIG_1(swigCPtr, y);
  }

  public void SetPositionZ(float z, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetPositionZ__SWIG_0(swigCPtr, z, (int)space);
  }

  public void SetPositionZ(float z) {
    SharpMochaPINVOKE.CTransform_SetPositionZ__SWIG_1(swigCPtr, z);
  }

  public void Translate(float x, float y, float z) {
    SharpMochaPINVOKE.CTransform_Translate__SWIG_0(swigCPtr, x, y, z);
  }

  public void Translate(MochaVec3 v) {
    SharpMochaPINVOKE.CTransform_Translate__SWIG_1(swigCPtr, MochaVec3.getCPtr(v));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetScale(MochaVec3 scale, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetScale__SWIG_0(swigCPtr, MochaVec3.getCPtr(scale), (int)space);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetScale(MochaVec3 scale) {
    SharpMochaPINVOKE.CTransform_SetScale__SWIG_1(swigCPtr, MochaVec3.getCPtr(scale));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetScale(float x, float y, float z, CTransform.TransformSpace space) {
    SharpMochaPINVOKE.CTransform_SetScale__SWIG_2(swigCPtr, x, y, z, (int)space);
  }

  public void SetScale(float x, float y, float z) {
    SharpMochaPINVOKE.CTransform_SetScale__SWIG_3(swigCPtr, x, y, z);
  }

  public MochaVec3 GetScale(CTransform.TransformSpace space) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetScale__SWIG_0(swigCPtr, (int)space), true);
    return ret;
  }

  public MochaVec3 GetScale() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.CTransform_GetScale__SWIG_1(swigCPtr), true);
    return ret;
  }

  [MochaDoNotShow]
  public CTransformVector children {
    set {
      SharpMochaPINVOKE.CTransform_children_set(swigCPtr, CTransformVector.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.CTransform_children_get(swigCPtr);
      CTransformVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new CTransformVector(cPtr, false);
      return ret;
    } 
  }

  public uint GetNumberOfChildren() {
    uint ret = SharpMochaPINVOKE.CTransform_GetNumberOfChildren(swigCPtr);
    return ret;
  }

  public CTransform GetChild(uint index) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.CTransform_GetChild(swigCPtr, index);
    CTransform ret = (cPtr == global::System.IntPtr.Zero) ? null : new CTransform(cPtr, false);
    return ret;
  }

  public void SetParent(CTransform parent) {
    SharpMochaPINVOKE.CTransform_SetParent(swigCPtr, CTransform.getCPtr(parent));
  }

  public CTransform GetParent() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.CTransform_GetParent(swigCPtr);
    CTransform ret = (cPtr == global::System.IntPtr.Zero) ? null : new CTransform(cPtr, false);
    return ret;
  }

  public enum TransformSpace {
    LOCAL,
    GLOBAL
  }

}

}
