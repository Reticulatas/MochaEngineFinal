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

public partial class GameObject : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal GameObject(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(GameObject obj) {
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

	public global::System.Runtime.InteropServices.HandleRef get_CPtr() {
		return swigCPtr;
	}

	public bool get_swigMemOwn()
	{
		return swigCMemOwn;
	}

	public GameObject(global::System.IntPtr cPtr) {
		swigCMemOwn = false;
		swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
	}

	public T RequireComponent<T>() where T : Component{
		Component from = RequireComponentByName(typeof(T).Name);
		System.Reflection.MethodInfo CPtrGetter = from.GetType().GetMethod("getCPtr", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
		return CPtrGetter == null ? default(T) : (T)System.Activator.CreateInstance
			(
			typeof(T),
			System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance,
			null,
			new object[] { ((HandleRef)CPtrGetter.Invoke(null, new object[] { from })).Handle, true},
			null
			);
	}

	public T GetComponentInChildren<T>() where T : Component{
		Component from = GetComponentInChildren(typeof(T).Name);
		if (from == null)
			return null;
		System.Reflection.MethodInfo CPtrGetter = from.GetType().GetMethod("getCPtr", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
		return CPtrGetter == null ? default(T) : (T)System.Activator.CreateInstance
			(
			typeof(T),
			System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance,
			null,
			new object[] { ((HandleRef)CPtrGetter.Invoke(null, new object[] { from })).Handle, true},
			null
			);
	}

	public T GetComponent<T>() where T : Component {
		Component from = GetComponentByName(typeof(T).Name, false);
		if (from == null)
			return null;
		System.Reflection.MethodInfo CPtrGetter = from.GetType().GetMethod("getCPtr", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
		return CPtrGetter == null ? default(T) : (T)System.Activator.CreateInstance
			(
			typeof(T),
			System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance,
			null,
			new object[] { ((HandleRef)CPtrGetter.Invoke(null, new object[] { from })).Handle, true},
			null
			);
	}

		public static bool operator==(GameObject g1, GameObject g2)
	{
		if (System.Object.ReferenceEquals(g1, g2))
			return true;
		if ((object)g1 == null || (object)g2 == null)
			return false;
		return g1.GetID() == g2.GetID();
	}
	public static bool operator!=(GameObject g1, GameObject g2)
	{
		return !(g1 == g2);
	}
	public override bool Equals(object obj)
	{
		if (!(obj is GameObject))
			return false;
		return ((obj as GameObject) == this);
	}
	public override int GetHashCode()
	{
		return GetID().GetHashCode();
	}

	public GameObject() { throw new InvalidOperationException("Cannot instantiate game object directly - use ObjectFactory"); }


  public TagSet flags {
    set {
      SharpMochaPINVOKE.GameObject_flags_set(swigCPtr, TagSet.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_flags_get(swigCPtr);
      TagSet ret = (cPtr == global::System.IntPtr.Zero) ? null : new TagSet(cPtr, false);
      return ret;
    } 
  }

  public GameObject Duplicate() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_Duplicate(swigCPtr);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    return ret;
  }

  public void Destroy() {
    SharpMochaPINVOKE.GameObject_Destroy(swigCPtr);
  }

  public void SetFlag(string f) {
    SharpMochaPINVOKE.GameObject_SetFlag(swigCPtr, f);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void ClearFlag(string f) {
    SharpMochaPINVOKE.GameObject_ClearFlag(swigCPtr, f);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool GetFlag(string f) {
    bool ret = SharpMochaPINVOKE.GameObject_GetFlag(swigCPtr, f);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public uint GetID() {
    uint ret = SharpMochaPINVOKE.GameObject_GetID(swigCPtr);
    return ret;
  }

  public bool HasComponentByName(string name) {
    bool ret = SharpMochaPINVOKE.GameObject_HasComponentByName(swigCPtr, name);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string GetName() {
    string ret = SharpMochaPINVOKE.GameObject_GetName(swigCPtr);
    return ret;
  }

  public string SetName(string s) {
    string ret = SharpMochaPINVOKE.GameObject_SetName(swigCPtr, s);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Component RequireComponentByName(string name) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_RequireComponentByName(swigCPtr, name);
    Component ret = (cPtr == global::System.IntPtr.Zero) ? null : new Component(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Component GetComponentByName(string name) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_GetComponentByName__SWIG_0(swigCPtr, name);
    Component ret = (cPtr == global::System.IntPtr.Zero) ? null : new Component(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Component GetComponentByName(string name, bool logerror) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_GetComponentByName__SWIG_1(swigCPtr, name, logerror);
    Component ret = (cPtr == global::System.IntPtr.Zero) ? null : new Component(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Component GetComponentInChildren(string name) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_GetComponentInChildren(swigCPtr, name);
    Component ret = (cPtr == global::System.IntPtr.Zero) ? null : new Component(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public CTransform transform {
    set {
      SharpMochaPINVOKE.GameObject_transform_set(swigCPtr, CTransform.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_transform_get(swigCPtr);
      CTransform ret = (cPtr == global::System.IntPtr.Zero) ? null : new CTransform(cPtr, false);
      return ret;
    } 
  }

  public string mName {
    set {
      SharpMochaPINVOKE.GameObject_mName_set(swigCPtr, value);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      string ret = SharpMochaPINVOKE.GameObject_mName_get(swigCPtr);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public string mGUID {
    set {
      SharpMochaPINVOKE.GameObject_mGUID_set(swigCPtr, value);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      string ret = SharpMochaPINVOKE.GameObject_mGUID_get(swigCPtr);
      if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public ComponentVector mComponents {
    set {
      SharpMochaPINVOKE.GameObject_mComponents_set(swigCPtr, ComponentVector.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_mComponents_get(swigCPtr);
      ComponentVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ComponentVector(cPtr, false);
      return ret;
    } 
  }

  public bool mIsSelected {
    set {
      SharpMochaPINVOKE.GameObject_mIsSelected_set(swigCPtr, value);
    } 
    get {
      bool ret = SharpMochaPINVOKE.GameObject_mIsSelected_get(swigCPtr);
      return ret;
    } 
  }

  public LayerMask mLayerMask {
    set {
      SharpMochaPINVOKE.GameObject_mLayerMask_set(swigCPtr, LayerMask.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_mLayerMask_get(swigCPtr);
      LayerMask ret = (cPtr == global::System.IntPtr.Zero) ? null : new LayerMask(cPtr, false);
      return ret;
    } 
  }

  public void LookAt(SWIGTYPE_p_Vector3 point) {
    SharpMochaPINVOKE.GameObject_LookAt__SWIG_0(swigCPtr, SWIGTYPE_p_Vector3.getCPtr(point));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public void LookAt(GameObject point) {
    SharpMochaPINVOKE.GameObject_LookAt__SWIG_1(swigCPtr, GameObject.getCPtr(point));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool GetIsDormantObject() {
    bool ret = SharpMochaPINVOKE.GameObject_GetIsDormantObject(swigCPtr);
    return ret;
  }

  public Prefab linkedPrefab {
    set {
      SharpMochaPINVOKE.GameObject_linkedPrefab_set(swigCPtr, Prefab.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_linkedPrefab_get(swigCPtr);
      Prefab ret = (cPtr == global::System.IntPtr.Zero) ? null : new Prefab(cPtr, false);
      return ret;
    } 
  }

  public GameObject RevertToPrefab() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_RevertToPrefab(swigCPtr);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    return ret;
  }

  public void WriteToPrefab() {
    SharpMochaPINVOKE.GameObject_WriteToPrefab(swigCPtr);
  }

  public void DetachFromPrefab() {
    SharpMochaPINVOKE.GameObject_DetachFromPrefab(swigCPtr);
  }

  public static GameObject GetGameObjectByName(string name_, bool errorIfNotFound) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_GetGameObjectByName__SWIG_0(name_, errorIfNotFound);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static GameObject GetGameObjectByName(string name_) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_GetGameObjectByName__SWIG_1(name_);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static GameObjectVector GetGameObjectsWithTag(string tag) {
    GameObjectVector ret = new GameObjectVector(SharpMochaPINVOKE.GameObject_GetGameObjectsWithTag(tag), true);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static GameObject GetGameObjectWithTag(string tag) {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.GameObject_GetGameObjectWithTag(tag);
    GameObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new GameObject(cPtr, false);
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
