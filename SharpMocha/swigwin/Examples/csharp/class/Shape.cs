/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class Shape : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Shape(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Shape obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Shape() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          examplePINVOKE.delete_Shape(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public double x {
    set {
      examplePINVOKE.Shape_x_set(swigCPtr, value);
    } 
    get {
      double ret = examplePINVOKE.Shape_x_get(swigCPtr);
      return ret;
    } 
  }

  public double y {
    set {
      examplePINVOKE.Shape_y_set(swigCPtr, value);
    } 
    get {
      double ret = examplePINVOKE.Shape_y_get(swigCPtr);
      return ret;
    } 
  }

  public void move(double dx, double dy) {
    examplePINVOKE.Shape_move(swigCPtr, dx, dy);
  }

  public virtual double area() {
    double ret = examplePINVOKE.Shape_area(swigCPtr);
    return ret;
  }

  public virtual double perimeter() {
    double ret = examplePINVOKE.Shape_perimeter(swigCPtr);
    return ret;
  }

  public static int nshapes {
    set {
      examplePINVOKE.Shape_nshapes_set(value);
    } 
    get {
      int ret = examplePINVOKE.Shape_nshapes_get();
      return ret;
    } 
  }

}
