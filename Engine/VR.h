#pragma once
#include "MathIncludes.h"
#include "RenderTarget.h"

class VR
{
public:

  VR();
  virtual ~VR();

  virtual bool Init() = 0;
  virtual bool StartUp() = 0;
  virtual void Release();
  virtual void RenderVR(CCamera* oculusCam) = 0;
  virtual void Resize();
  virtual bool IsConnected() = 0;
  virtual unsigned GetRecommendedWidth();
  virtual unsigned GetRecommendedHeight();
  virtual Vector3 GetEyePos(unsigned index);
  virtual Vector3 GetEyePos();
  virtual Matrix4 GetProjMatrix(unsigned index);
  virtual Matrix4 GetViewMatrix(unsigned index);

protected:

  RenderTarget mVR_RT;
  Vector3      mEyePos[2];
  Matrix4      mView[2];
  Matrix4      mProj[2];
  Matrix4      mOrthoProj[2];
  int          mCurrentEyeIndex;
  int          mRecommendedWidth;
  int          mRecommendedHeight;
  Rect         mFinalVR_VP;
  bool         mVsyncEnabled;
};