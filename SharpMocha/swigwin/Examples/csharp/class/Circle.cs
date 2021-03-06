/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class Circle : Shape {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal Circle(global::System.IntPtr cPtr, bool cMemoryOwn) : base(examplePINVOKE.Circle_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Circle obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Circle() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          examplePINVOKE.delete_Circle(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public Circle(double r) : this(examplePINVOKE.new_Circle(r), true) {
  }

  public override double area() {
    double ret = examplePINVOKE.Circle_area(swigCPtr);
    return ret;
  }

  public override double perimeter() {
    double ret = examplePINVOKE.Circle_perimeter(swigCPtr);
    return ret;
  }

}
