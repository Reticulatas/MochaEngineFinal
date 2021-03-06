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

public class MochaVec3 : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal MochaVec3(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(MochaVec3 obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~MochaVec3() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          SharpMochaPINVOKE.delete_MochaVec3(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }


		public override string ToString()
		{
			return "(" + x.ToString() + ", " + y.ToString() + ", " + z.ToString() + ")";
		}
		public MochaVec3(Microsoft.Xna.Framework.Vector3 vec) : this(vec.X, vec.Y, vec.Z)
		{
		}
		public static implicit operator MochaVec3(Microsoft.Xna.Framework.Vector3 vec)  // explicit byte to digit conversion operator
		{
			return new MochaVec3(vec);
		}
		public static implicit operator Microsoft.Xna.Framework.Vector3(MochaVec3 vec)  // explicit byte to digit conversion operator
		{
			Microsoft.Xna.Framework.Vector3 tmp = new Microsoft.Xna.Framework.Vector3();

			tmp.X = vec.x;
			tmp.Y = vec.y;
			tmp.Z = vec.z;

			return tmp;
		}
	
  public MochaVec3() : this(SharpMochaPINVOKE.new_MochaVec3__SWIG_0(), true) {
  }

  public MochaVec3(float x, float y, float z) : this(SharpMochaPINVOKE.new_MochaVec3__SWIG_1(x, y, z), true) {
  }

  public MochaVec3(float xyz) : this(SharpMochaPINVOKE.new_MochaVec3__SWIG_2(xyz), true) {
  }

  public MochaVec3(MochaVec2 Vector2, float z) : this(SharpMochaPINVOKE.new_MochaVec3__SWIG_3(MochaVec2.getCPtr(Vector2), z), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public MochaVec3(MochaVec2 Vector2) : this(SharpMochaPINVOKE.new_MochaVec3__SWIG_4(MochaVec2.getCPtr(Vector2)), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public MochaVec3(SWIGTYPE_p_ConstRealPointer data) : this(SharpMochaPINVOKE.new_MochaVec3__SWIG_5(SWIGTYPE_p_ConstRealPointer.getCPtr(data)), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public MochaVec3(MochaVec3 Vector3) : this(SharpMochaPINVOKE.new_MochaVec3__SWIG_6(MochaVec3.getCPtr(Vector3)), true) {
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public SWIGTYPE_p_float ToFloats() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.MochaVec3_ToFloats(swigCPtr);
    SWIGTYPE_p_float ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_float(cPtr, false);
    return ret;
  }

  public float GetE(uint index) {
    float ret = SharpMochaPINVOKE.MochaVec3_GetE(swigCPtr, index);
    return ret;
  }

  public MochaVec3 Sub() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Sub__SWIG_0(swigCPtr), true);
    return ret;
  }

  public MochaVec3 Mul(float rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Mul__SWIG_0(swigCPtr, rhs), true);
    return ret;
  }

  public MochaVec3 Div(float rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Div__SWIG_0(swigCPtr, rhs), true);
    return ret;
  }

  public MochaVec3 Add(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Add(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec3 Sub(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Sub__SWIG_1(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool Comp(MochaVec3 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec3_Comp(swigCPtr, MochaVec3.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool NegComp(MochaVec3 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec3_NegComp(swigCPtr, MochaVec3.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec3 Mul(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Mul__SWIG_1(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec3 Div(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Div__SWIG_1(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Set(float x, float y, float z) {
    SharpMochaPINVOKE.MochaVec3_Set(swigCPtr, x, y, z);
  }

  public void Splat(float xyz) {
    SharpMochaPINVOKE.MochaVec3_Splat(swigCPtr, xyz);
  }

  public void ScaleByVector(MochaVec3 rhs) {
    SharpMochaPINVOKE.MochaVec3_ScaleByVector(swigCPtr, MochaVec3.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void ZeroOut() {
    SharpMochaPINVOKE.MochaVec3_ZeroOut(swigCPtr);
  }

  public MochaVec3 Reflect(MochaVec3 reflectionAxis) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Reflect(swigCPtr, MochaVec3.getCPtr(reflectionAxis)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public MochaVec3 Project(MochaVec3 axis) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Project(swigCPtr, MochaVec3.getCPtr(axis)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void AddScaledVector(MochaVec3 Vector, float scalar) {
    SharpMochaPINVOKE.MochaVec3_AddScaledVector(swigCPtr, MochaVec3.getCPtr(Vector), scalar);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public float Dot(MochaVec3 rhs) {
    float ret = SharpMochaPINVOKE.MochaVec3_Dot(swigCPtr, MochaVec3.getCPtr(rhs));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float Length() {
    float ret = SharpMochaPINVOKE.MochaVec3_Length(swigCPtr);
    return ret;
  }

  public float LengthSq() {
    float ret = SharpMochaPINVOKE.MochaVec3_LengthSq(swigCPtr);
    return ret;
  }

  public MochaVec3 Normalized() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Normalized(swigCPtr), true);
    return ret;
  }

  public float Normalize() {
    float ret = SharpMochaPINVOKE.MochaVec3_Normalize(swigCPtr);
    return ret;
  }

  public void Ceil() {
    SharpMochaPINVOKE.MochaVec3_Ceil(swigCPtr);
  }

  public void Floor() {
    SharpMochaPINVOKE.MochaVec3_Floor(swigCPtr);
  }

  public void Truncate() {
    SharpMochaPINVOKE.MochaVec3_Truncate(swigCPtr);
  }

  public void RoundToExtremes() {
    SharpMochaPINVOKE.MochaVec3_RoundToExtremes(swigCPtr);
  }

  public void Round() {
    SharpMochaPINVOKE.MochaVec3_Round(swigCPtr);
  }

  public float AttemptNormalize() {
    float ret = SharpMochaPINVOKE.MochaVec3_AttemptNormalize(swigCPtr);
    return ret;
  }

  public MochaVec3 Negate() {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Negate(swigCPtr), false);
    return ret;
  }

  public bool Valid() {
    bool ret = SharpMochaPINVOKE.MochaVec3_Valid(swigCPtr);
    return ret;
  }

  public MochaVec3 Cross(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Cross(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void InvertComponents() {
    SharpMochaPINVOKE.MochaVec3_InvertComponents(swigCPtr);
  }

  public float Distance(MochaVec3 rhs) {
    float ret = SharpMochaPINVOKE.MochaVec3_Distance(swigCPtr, MochaVec3.getCPtr(rhs));
    return ret;
  }

  public MochaVec3 Cross2d(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Cross2d(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    return ret;
  }

  public MochaVec3 Min(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Min(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    return ret;
  }

  public MochaVec3 Max(MochaVec3 rhs) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Max(swigCPtr, MochaVec3.getCPtr(rhs)), true);
    return ret;
  }

  public MochaVec3 Lerp(MochaVec3 end, float tValue) {
    MochaVec3 ret = new MochaVec3(SharpMochaPINVOKE.MochaVec3_Lerp(swigCPtr, MochaVec3.getCPtr(end), tValue), true);
    return ret;
  }

  public void Clamp(float min, float max) {
    SharpMochaPINVOKE.MochaVec3_Clamp(swigCPtr, min, max);
  }

  public bool AllLess(MochaVec3 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec3_AllLess(swigCPtr, MochaVec3.getCPtr(rhs));
    return ret;
  }

  public bool AnyLess(MochaVec3 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec3_AnyLess(swigCPtr, MochaVec3.getCPtr(rhs));
    return ret;
  }

  public bool AllGreater(MochaVec3 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec3_AllGreater(swigCPtr, MochaVec3.getCPtr(rhs));
    return ret;
  }

  public bool AnyGreater(MochaVec3 rhs) {
    bool ret = SharpMochaPINVOKE.MochaVec3_AnyGreater(swigCPtr, MochaVec3.getCPtr(rhs));
    return ret;
  }

  public float x {
    set {
      SharpMochaPINVOKE.MochaVec3_x_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.MochaVec3_x_get(swigCPtr);
      return ret;
    } 
  }

  public float y {
    set {
      SharpMochaPINVOKE.MochaVec3_y_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.MochaVec3_y_get(swigCPtr);
      return ret;
    } 
  }

  public float z {
    set {
      SharpMochaPINVOKE.MochaVec3_z_set(swigCPtr, value);
    } 
    get {
      float ret = SharpMochaPINVOKE.MochaVec3_z_get(swigCPtr);
      return ret;
    } 
  }

  public SWIGTYPE_p_float array {
    set {
      SharpMochaPINVOKE.MochaVec3_array_set(swigCPtr, SWIGTYPE_p_float.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.MochaVec3_array_get(swigCPtr);
      SWIGTYPE_p_float ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_float(cPtr, false);
      return ret;
    } 
  }

}

}
