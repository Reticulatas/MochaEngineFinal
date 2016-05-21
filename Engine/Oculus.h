#pragma once

#include "OVR_CAPI.h"
#define OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h"
#include "Kernel/OVR_Math.h"
#include "VR.h"

class RenderOculusTask;

class OculusVR : public VR
{
public:
  OculusVR();
  virtual ~OculusVR();

  bool Init();
  bool StartUp();
  void Release();
  void RenderVR(CCamera* oculusCam);
  void Resize();
  bool IsConnected();
  //unsigned GetRecommendedWidth();
  //unsigned GetRecommendedHeight();
  //Vector3 GetEyePos(unsigned index);
  //Vector3 GetEyePos();
  //Matrix4 GetProjMatrix(unsigned index);
  //Matrix4 GetViewMatrix(unsigned index);

  // Math Helper
  Quaternion Convert_OVRQuat(const OVR::Quat<float>& q);
  Vector3 Convert_OVRVector3f(const OVR::Vector3f& v);
  Rect Convert_OVRRecti(const OVR::Recti& r);

  ovrHmd GetOvrHmd() const;
  //ovrTexture* GetOvrTexture();
  //ovrPosef* GetOvrEyeRenderPos();
  //BarristaToGo::BarristaToGoDescriptor* OculusTaskDesc() const { return mOculusTaskDesc; }

  static OculusVR* getInstance();

private:

  void SafetyWarning();
  OVR::Matrix4f OculusVR::CalculateViewFromPose(const OVR::Posef& pose, CTransform* transform);

  bool mIsLowPersistence;
  bool mDynamicPrediction;
  bool mIsDisplayoff;
  bool mIsMirrorToWindow;

  bool mSupportSRGB;
  bool mPixelLuminanceOverdrive;
  bool mTimewarpEnabled;
  bool mTimewarpNoJitEnabled;
  bool mHqAaDistortion;

  bool mPositionTrackingEnabled;
  bool mIsDirectMode;

  ovrHmd           mOvrHmd;
  ovrEyeRenderDesc mEyeRenderDesc[2];
  ovrRecti         mEyeRenderViewport[2];
  ovrD3D11Texture  mEyeTexture[2];
  //OVR::Sizei       mRecommendedSize;  // Recommended Size
  //RenderTarget     mOculusRT;
  //Vector3          mEyePos[2];
  //Matrix4          mView[2];
  //Matrix4          mProj[2];
  //Matrix4          mOrthoProj[2];
  //int              mCurrentEyeIndex;

  //ovrPosef         mEyeRenderPose[2];
  //RenderOculusTask* mOculusTask;
  //BarristaToGo::BarristaToGoDescriptor* mOculusTaskDesc;
};