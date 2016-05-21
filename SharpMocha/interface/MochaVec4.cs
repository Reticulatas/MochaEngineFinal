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

public class MochaVec4 : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal MochaVec4(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(MochaVec4 obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~MochaVec4() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_MochaVec4(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

		public MochaVec4(Microsoft.Xna.Framework.Vector4 vec)
		{
			x = vec.X;
            y = vec.Y;
            z = vec.Z;
			w = vec.W;
		}
		public static implicit operator MochaVec4(Microsoft.Xna.Framework.Vector4 vec)  // explicit byte to digit conversion operator
		{
			return new MochaVec4(vec);
		}
		public static implicit operator Microsoft.Xna.Framework.Vector4(MochaVec4 vec)  // explicit byte to digit conversion operator
		{
			Microsoft.Xna.Framework.Vector4 tmp = new Microsoft.Xna.Framework.Vector4();
			
			tmp.X = vec.x;
			tmp.Y = vec.y;
			tmp.Z = vec.z;
			tmp.W = vec.w;
			
			return tmp;
		}
	    
  public MochaVec4() : this(SharpMochaPINVOKE.new_MochaVec4__SWIG_0(), true) {
  }

  public MochaVec4(float x, float y, float z, float w) : this(SharpMochaPINVOKE.new_MochaVec4__SWIG_1(x, y, z, w), true) {
  }

  public MochaVec4(float x, float y, float z) : this(SharpMochaPINVOKE.new_MochaVec4__SWIG_2(x, y, z), true) {
  }

  public MochaVec4(SWIGTYPE_p_ConstRealPointer data) : this(SharpMochaPINVOKE.new_MochaVec4__SWIG_3(SWIGTYPE_p_ConstRealPointer.getCPtr(data)), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public MochaVec4(float xyzw) : this(SharpMochaPINVOKE.new_MochaVec4__SWIG_4(xyzw), true) {
  }

  public SWIGTYPE_p_float GetE(uint index) {
    SWIGTYPE_p_float ret = new SWIGTYPE_p_float(SharpMochaPINVOKE.MochaVec4_GetE(swigCPtr, index), false);
    return ret;
  }

  public MochaVec4 Sub() {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Sub__SWIG_0(swigCPtr), true);
    return ret;
  }

  public MochaVec4 Mul(float rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Mul__SWIG_0(swigCPtr, rhs), true);
    return ret;
  }

  public MochaVec4 Div(float rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Div__SWIG_0(swigCPtr, rhs), true);
    return ret;
  }

  public MochaVec4 Add(MochaVec4 rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Add(swigCPtr, MochaVec4.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec4 Sub(MochaVec4 rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Sub__SWIG_1(swigCPtr, MochaVec4.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool Comp(MochaVec4 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec4_Comp(swigCPtr, MochaVec4.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool NegComp(MochaVec4 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec4_NegComp(swigCPtr, MochaVec4.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec4 Mul(MochaVec4 rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Mul__SWIG_1(swigCPtr, MochaVec4.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec4 Div(MochaVec4 rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Div__SWIG_1(swigCPtr, MochaVec4.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Set(float x, float y, float z, float w) {
    SharpMochaPINVOKE.MochaVec4_Set__SWIG_0(swigCPtr, x, y, z, w);
  }

  public void Set(float x, float y, float z) {
    SharpMochaPINVOKE.MochaVec4_Set__SWIG_1(swigCPtr, x, y, z);
  }

  public void Splat(float xyzw) {
    SharpMochaPINVOKE.MochaVec4_Splat(swigCPtr, xyzw);
  }

  public void ScaleByVector(MochaVec4 rhs) {
    SharpMochaPINVOKE.MochaVec4_ScaleByVector(swigCPtr, MochaVec4.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public MochaVec4 ScaledByVector(MochaVec4 rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_ScaledByVector(swigCPtr, MochaVec4.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void ZeroOut() {
    SharpMochaPINVOKE.MochaVec4_ZeroOut(swigCPtr);
  }

  public void AddScaledVector(MochaVec4 Vector, float scalar) {
    SharpMochaPINVOKE.MochaVec4_AddScaledVector(swigCPtr, MochaVec4.getCPtr(Vector), scalar);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public float Dot(MochaVec4 rhs) {
    float ret = SharpMochaPINVOKE.MochaVec4_Dot(swigCPtr, MochaVec4.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float Length() {
    float ret = SharpMochaPINVOKE.MochaVec4_Length(swigCPtr);
    return ret;
  }

  public float LengthSq() {
    float ret = SharpMochaPINVOKE.MochaVec4_LengthSq(swigCPtr);
    return ret;
  }

  public MochaVec4 Normalized() {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Normalized(swigCPtr), true);
    return ret;
  }

  public float Normalize() {
    float ret = SharpMochaPINVOKE.MochaVec4_Normalize(swigCPtr);
    return ret;
  }

  public float AttemptNormalize() {
    float ret = SharpMochaPINVOKE.MochaVec4_AttemptNormalize(swigCPtr);
    return ret;
  }

  public MochaVec4 Negate() {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Negate(swigCPtr), false);
    return ret;
  }

  public bool Valid() {
    bool ret = SharpMochaPINVOKE.MochaVec4_Valid(swigCPtr);
    return ret;
  }

  public MochaVec4 Cross(MochaVec4 rhs) {
    MochaVec4 ret = new MochaVec4(SharpMochaPINVOKE.MochaVec4_Cross(swigCPtr, MochaVec4.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float x {
    set {
      SharpMochaPINVOKE.MochaVec4_x_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.MochaVec4_x_get(swigCPtr);
      return ret;
    } 
  }

  public float y {
    set {
      SharpMochaPINVOKE.MochaVec4_y_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.MochaVec4_y_get(swigCPtr);
      return ret;
    } 
  }

  public float z {
    set {
      SharpMochaPINVOKE.MochaVec4_z_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.MochaVec4_z_get(swigCPtr);
      return ret;
    } 
  }

  public float w {
    set {
      SharpMochaPINVOKE.MochaVec4_w_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.MochaVec4_w_get(swigCPtr);
      return ret;
    } 
  }

  public SWIGTYPE_p_float array {
    set {
      SharpMochaPINVOKE.MochaVec4_array_set(swigCPtr, SWIGTYPE_p_float.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.MochaVec4_array_get(swigCPtr);
      SWIGTYPE_p_float ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_float(cPtr, false);
      return ret;
    } 
  }

}

}