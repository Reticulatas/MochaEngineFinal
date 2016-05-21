#pragma once
#include "steam_api.h"
#include "steamvr.h"
#include "VR.h"

#define EyeCount 2

class SteamVR : public VR
{
public:

  SteamVR();
  virtual ~SteamVR();

  bool Init();
  bool StartUp();
  void Release();
  bool IsConnected();
  void RenderVR(CCamera* oculusCam);

  vr::IHmd* GetHMD() const;

  static SteamVR* getInstance();

private:

  void CreateDistortionTexture();
  void UpdateCamFromTracker(CCamera* cam, int eye);
  void GetViewportFromHmd(int eye, Rect& r);

  vr::IHmd* mVRHmd;
  RenderTarget mDistortionRT[2];
};

//-----------------------------------------------------------------------------
// Purpose: callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
//extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
//{
//  // if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
//  // if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
//  Log(pchDebugText);
//
//  if (nSeverity >= 1)
//  {
//    // place to set a breakpoint for catching API errors
//    int x = 3;
//    x = x;
//  }
//}