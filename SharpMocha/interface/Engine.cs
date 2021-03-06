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

public partial class Engine : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Engine(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Engine obj) {
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

  public void StartFromConfig(bool run) {
    SharpMochaPINVOKE.Engine_StartFromConfig__SWIG_0(swigCPtr, run);
  }

  public void StartFromConfig() {
    SharpMochaPINVOKE.Engine_StartFromConfig__SWIG_1(swigCPtr);
  }

  public void Start(bool showConsole, uint width, uint height, bool fullscreen, bool run) {
    SharpMochaPINVOKE.Engine_Start__SWIG_0(swigCPtr, showConsole, width, height, fullscreen, run);
  }

  public void Start(bool showConsole, uint width, uint height, bool fullscreen) {
    SharpMochaPINVOKE.Engine_Start__SWIG_1(swigCPtr, showConsole, width, height, fullscreen);
  }

  public void Run() {
    SharpMochaPINVOKE.Engine_Run(swigCPtr);
  }

  public void RegisterEditorMode() {
    SharpMochaPINVOKE.Engine_RegisterEditorMode(swigCPtr);
  }

  public void LoadInitialLevel() {
    SharpMochaPINVOKE.Engine_LoadInitialLevel(swigCPtr);
  }

  public void LockMainLoop() {
    SharpMochaPINVOKE.Engine_LockMainLoop(swigCPtr);
  }

  public void UnlockMainLoop() {
    SharpMochaPINVOKE.Engine_UnlockMainLoop(swigCPtr);
  }

  public void GetDesktopResolution(SWIGTYPE_p_int horizontal, SWIGTYPE_p_int vertical) {
    SharpMochaPINVOKE.Engine_GetDesktopResolution(swigCPtr, SWIGTYPE_p_int.getCPtr(horizontal), SWIGTYPE_p_int.getCPtr(vertical));
    if (SharpMochaPINVOKE.SWIGPendingException.Pending) throw SharpMochaPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool GetIsInited() {
    bool ret = SharpMochaPINVOKE.Engine_GetIsInited(swigCPtr);
    return ret;
  }

  public bool GetIsEditorMode() {
    bool ret = SharpMochaPINVOKE.Engine_GetIsEditorMode(swigCPtr);
    return ret;
  }

  public void Exit() {
    SharpMochaPINVOKE.Engine_Exit(swigCPtr);
  }

  public MochaWindow GetGameWnd() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.Engine_GetGameWnd(swigCPtr);
    MochaWindow ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaWindow(cPtr, false);
    return ret;
  }

  public MochaWindow GetVRWnd() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.Engine_GetVRWnd(swigCPtr);
    MochaWindow ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaWindow(cPtr, false);
    return ret;
  }

  public MochaWindow GetFocusedWnd() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.Engine_GetFocusedWnd(swigCPtr);
    MochaWindow ret = (cPtr == global::System.IntPtr.Zero) ? null : new MochaWindow(cPtr, false);
    return ret;
  }

  public void RegisterSafeFrameToBarrista() {
    SharpMochaPINVOKE.Engine_RegisterSafeFrameToBarrista(swigCPtr);
  }

  public void SignalSafeFrameContinue() {
    SharpMochaPINVOKE.Engine_SignalSafeFrameContinue(swigCPtr);
  }

  public bool IsInSameFrame() {
    bool ret = SharpMochaPINVOKE.Engine_IsInSameFrame(swigCPtr);
    return ret;
  }

  public bool IsGameInFocus() {
    bool ret = SharpMochaPINVOKE.Engine_IsGameInFocus(swigCPtr);
    return ret;
  }

  public bool IsEditorWindowInFocus() {
    bool ret = SharpMochaPINVOKE.Engine_IsEditorWindowInFocus(swigCPtr);
    return ret;
  }

  public void SetPauseMode(bool val) {
    SharpMochaPINVOKE.Engine_SetPauseMode(swigCPtr, val);
  }

  public static Engine getInstance() {
    global::System.IntPtr cPtr = SharpMochaPINVOKE.Engine_getInstance();
    Engine ret = (cPtr == global::System.IntPtr.Zero) ? null : new Engine(cPtr, false);
    return ret;
  }

  public bool mIsLetterBox {
    set {
      SharpMochaPINVOKE.Engine_mIsLetterBox_set(swigCPtr, value);
    } 
    get {
      bool ret = SharpMochaPINVOKE.Engine_mIsLetterBox_get(swigCPtr);
      return ret;
    } 
  }

}

}
