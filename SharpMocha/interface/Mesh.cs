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

public partial class Mesh : Asset {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal Mesh(global::System.IntPtr cPtr, bool cMemoryOwn) : base(SharpMochaPINVOKE.Mesh_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Mesh obj) {
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

  public string GetMeshName() {
    string ret = SharpMochaPINVOKE.Mesh_GetMeshName(swigCPtr);
    return ret;
  }

  public VertexVector GetVertices() {
    VertexVector ret = new VertexVector(SharpMochaPINVOKE.Mesh_GetVertices(swigCPtr), false);
    return ret;
  }

  public IntVector GetIndices() {
    IntVector ret = new IntVector(SharpMochaPINVOKE.Mesh_GetIndices(swigCPtr), false);
    return ret;
  }

  public SWIGTYPE_p_MeshType GetMeshType() {
    SWIGTYPE_p_MeshType ret = new SWIGTYPE_p_MeshType(SharpMochaPINVOKE.Mesh_GetMeshType(swigCPtr), true);
    return ret;
  }

  public uint GetVertexCount() {
    uint ret = SharpMochaPINVOKE.Mesh_GetVertexCount(swigCPtr);
    return ret;
  }

  public AABB GetAABB() {
    AABB ret = new AABB(SharpMochaPINVOKE.Mesh_GetAABB(swigCPtr), true);
    return ret;
  }

}

}