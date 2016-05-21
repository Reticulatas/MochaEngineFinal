#include "stdafx.h"
#include "Oculus.h"
#include "Renderer.h"
#include "Input.h"
#include "RenderTasks.h"
#include "ThreadUtils.h"

#define HSWDISPLAY_SCALE 0.92f

OculusVR::OculusVR()
{
	// mOculusTask = 0;
	// mOculusTaskDesc = 0;
	mOvrHmd = 0;
	
	mIsLowPersistence = true;
	mDynamicPrediction = true;
	mIsDisplayoff = false;
	mIsMirrorToWindow = true;

	mSupportSRGB = true;
	mPixelLuminanceOverdrive = true;
	mTimewarpEnabled = true;
	mTimewarpNoJitEnabled = false;
	mHqAaDistortion = true;

	mPositionTrackingEnabled = true;
  mIsDirectMode = false;
}

bool OculusVR::Init()
{
  // Initializes LibOVR, and the Rift
  ovr_Initialize();
  if (!mOvrHmd)
  {
    mOvrHmd = ovrHmd_Create(0);
    if (!mOvrHmd)
    {
      //MessageBoxA(NULL, "Oculus Rift not detected.", "", MB_OK);
      std::cout << "Oculus Rift not detected." << std::endl;
      return false;
    }
    if (mOvrHmd->ProductName[0] == '\0')
      std::cout << "Rift detected, display not enabled" << std::endl;
    //  MessageBoxA(NULL, "Rift detected, display not enabled.", "", MB_OK);
  }

  return true;
}

bool OculusVR::StartUp()
{
  if (!mOvrHmd)
    return false;

  //Setup Window and Graphics - use window frame if relying on Oculus driver
  const int backBufferMultisample = 1;
  mIsDirectMode = (mOvrHmd->HmdCaps & ovrHmdCap_ExtendDesktop) ? false : true;

  HWND window = Engine::getInstance()->GetVRWnd()->GetHWND();
  if (!window)
    return false;
  ovrHmd_AttachToWindow(mOvrHmd, window, NULL, NULL);

  //Configure Stereo settings.
  OVR::Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(mOvrHmd, ovrEye_Left, mOvrHmd->DefaultEyeFov[0], 1.0f);
  OVR::Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(mOvrHmd, ovrEye_Right, mOvrHmd->DefaultEyeFov[1], 1.0f);

  //mRecommendedWidth = recommenedTex0Size.w + recommenedTex1Size.w;
  mRecommendedWidth = recommenedTex0Size.w;
  mRecommendedHeight = max(recommenedTex0Size.h, recommenedTex1Size.h);

  Renderer* pRender = Renderer::getInstance();
  pRender->OnResize();
  mRecommendedWidth *= 2; // Left + Right texture width
  mVR_RT.InitRenderTarget(mRecommendedWidth, mRecommendedHeight, Renderer::getInstance()->GetFrameBufferFormat());

  // Initialize eye rendering information.
  // The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
  ovrFovPort eyeFov[2] = { mOvrHmd->DefaultEyeFov[0], mOvrHmd->DefaultEyeFov[1] };

  mEyeRenderViewport[0].Pos = OVR::Vector2i(0, 0);
  mEyeRenderViewport[0].Size = OVR::Sizei(mRecommendedWidth / 2, mRecommendedHeight);
  mEyeRenderViewport[1].Pos = OVR::Vector2i((mRecommendedWidth + 1) / 2, 0);
  mEyeRenderViewport[1].Size = mEyeRenderViewport[0].Size;
  mFinalVR_VP.width = mRecommendedWidth;
  mFinalVR_VP.height = mRecommendedHeight;

  // Query D3D texture data.
  mEyeTexture[0].D3D11.Header.API = ovrRenderAPI_D3D11;
  mEyeTexture[0].D3D11.Header.TextureSize = OVR::Sizei(mRecommendedWidth, mRecommendedHeight);
  mEyeTexture[0].D3D11.Header.RenderViewport = mEyeRenderViewport[0];
  mEyeTexture[0].D3D11.pTexture = mVR_RT.GetTexture();
  mEyeTexture[0].D3D11.pSRView = mVR_RT.GetShaderResourceView();

  mRecommendedWidth /= 2;

  // Right eye uses the same texture, but different rendering viewport.
  mEyeTexture[1] = mEyeTexture[0];
  mEyeTexture[1].D3D11.Header.RenderViewport = mEyeRenderViewport[1];

  // Configure d3d11.
  ovrD3D11Config d3d11cfg;
  d3d11cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
  d3d11cfg.D3D11.Header.RTSize = OVR::Sizei(mOvrHmd->Resolution.w, mOvrHmd->Resolution.h);
  d3d11cfg.D3D11.Header.Multisample = backBufferMultisample;
  d3d11cfg.D3D11.pDevice = pRender->md3dDevice;
  d3d11cfg.D3D11.pDeviceContext = pRender->md3dImmediateContext;
  d3d11cfg.D3D11.pBackBufferRT = pRender->mVRFinalRT.GetRenderTargetView();
  d3d11cfg.D3D11.pSwapChain = pRender->mOculusSwapChain;

  // Configure DistortionCap
  unsigned distortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette;

  if (mSupportSRGB)
    distortionCaps |= ovrDistortionCap_SRGB;
  if (mPixelLuminanceOverdrive)
    distortionCaps |= ovrDistortionCap_Overdrive;
  if (mTimewarpEnabled)
    distortionCaps |= ovrDistortionCap_TimeWarp;
  if (mTimewarpNoJitEnabled)
    distortionCaps |= ovrDistortionCap_ProfileNoTimewarpSpinWaits;
  if (mHqAaDistortion)
    distortionCaps |= ovrDistortionCap_HqDistortion;

  if (!ovrHmd_ConfigureRendering(mOvrHmd, &d3d11cfg.Config, distortionCaps,
    eyeFov, mEyeRenderDesc))	return false;

  // Configure hmd caps
  unsigned hmdCaps = (mVsyncEnabled ? 0 : ovrHmdCap_NoVSync);

  if (mIsLowPersistence)
    hmdCaps |= ovrHmdCap_LowPersistence;
  // ovrHmdCap_DynamicPrediction - enables internal latency feedback
  if (mDynamicPrediction)
    hmdCaps |= ovrHmdCap_DynamicPrediction;
  // ovrHmdCap_DisplayOff - turns off the display
  if (mIsDisplayoff)
    hmdCaps |= ovrHmdCap_DisplayOff;
  if (!mIsMirrorToWindow)
    hmdCaps |= ovrHmdCap_NoMirrorToWindow;

  ovrHmd_SetEnabledCaps(mOvrHmd, hmdCaps);

  // Start the sensor which informs of the Rift's pose and motion
  unsigned sensorCaps = ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection;

  if (mPositionTrackingEnabled)
    sensorCaps |= ovrTrackingCap_Position;

  ovrHmd_ConfigureTracking(mOvrHmd, sensorCaps, 0);

  // Calculate projections
  OVR::Matrix4f Projection[2];
  Projection[0] = ovrMatrix4f_Projection(mEyeRenderDesc[0].Fov, 0.1f, 1000.0f, false);
  Projection[1] = ovrMatrix4f_Projection(mEyeRenderDesc[1].Fov, 0.1f, 1000.0f, false);
  mProj[0] = Matrix4(&Projection[0].M[0][0]);
  mProj[1] = Matrix4(&Projection[1].M[0][0]);

  OVR::Matrix4f OrthoProjection[2];
  float    orthoDistance = .8f; // 2D is 0.8 meter from camera
  OVR::Vector2f orthoScale0 = OVR::Vector2f(1.0f) / OVR::Vector2f(mEyeRenderDesc[0].PixelsPerTanAngleAtCenter);
  OVR::Vector2f orthoScale1 = OVR::Vector2f(1.0f) / OVR::Vector2f(mEyeRenderDesc[1].PixelsPerTanAngleAtCenter);
  OrthoProjection[0] = ovrMatrix4f_OrthoSubProjection(Projection[0], orthoScale0, orthoDistance, mEyeRenderDesc[0].HmdToEyeViewOffset.x);
  OrthoProjection[1] = ovrMatrix4f_OrthoSubProjection(Projection[1], orthoScale1, orthoDistance, mEyeRenderDesc[1].HmdToEyeViewOffset.x);

  //OrthoProjection[0].M[0][3] *= Engine::getInstance()->GetGameWnd()->GetWindowWidth() / Engine::getInstance()->GetGameWnd()->GetWindowHeight();
  //OrthoProjection[1].M[0][3] *= Engine::getInstance()->GetGameWnd()->GetWindowWidth() / Engine::getInstance()->GetGameWnd()->GetWindowHeight();

  mOrthoProj[0] = Matrix4(&OrthoProjection[0].M[0][0]);
  mOrthoProj[1] = Matrix4(&OrthoProjection[1].M[0][0]);

  //mOculusTask = new RenderOculusTask();
  return true;
}

void OculusVR::RenderVR(CCamera* vrCam)
{
  SafetyWarning();

  Renderer* pRender = Renderer::getInstance();
  static ovrPosef eyeRenderPose[2];

  // Start timing
  ovrHmd_BeginFrame(mOvrHmd, 0);

  //mOculusTask->oculusCam = oculusCam;
  //mOculusTask->oculusRT = &mOculusRT;
  //mOculusTask->promise = std::promise<void>();
  //mOculusTask->future = mOculusTask->promise.get_future();
  //mOculusTaskDesc = barrista().togo->Invoke(mOculusTask, MAKEMETAMETHODVARIABLE(RenderOculusTask, RenderOculus), 0);
  //{
  //  ThreadUtils::ThreadLockVisitor tv(6);
  //  mOculusTask->future.wait();

    // Adjust eye position and rotation from controls, maintaining y position from HMD.
    static ovrTrackingState HmdState;

    ovrVector3f hmdToEyeViewOffset[2] = { mEyeRenderDesc[0].HmdToEyeViewOffset, mEyeRenderDesc[1].HmdToEyeViewOffset };
    ovrHmd_GetEyePoses(mOvrHmd, 0, hmdToEyeViewOffset, eyeRenderPose, &HmdState);

    //HeadPos.y = ovrHmd_GetFloat(mOvrHmd, OVR_KEY_EYE_HEIGHT, HeadPos.y);

    // Must clear to black
    pRender->ClearFrameBuffer(mVR_RT.GetRenderTargetView(), COLOR::BLACK);

	  CTransform* camTransform = vrCam->gameObject->transform;
	  Vector3 origCamPos = camTransform->GetGlobalPosition();
    //Quaternion origCamRot = camTransform->GetRotationQuaternion();

    for (int eyeIndex = 0; eyeIndex < ovrEye_Count; ++eyeIndex)
    {
      ovrEyeType eye = mOvrHmd->EyeRenderOrder[eyeIndex];
      mCurrentEyeIndex = eye;

      Quaternion orientation = Convert_OVRQuat(eyeRenderPose[eye].Orientation);
      // Need to invert and negate z for LHS
      orientation.Invert();
      orientation.z *= -1;
	  
      // Concatenate cam's rotation and oculus tracking rotation
      //orientation = ToQuaternion(camTransform->rotation) * orientation; // Commented out because of oculus frustum culling
      //camTransform->rotation = ToEulerAngles(orientation, camTransform->rotation.Order.Order);
      //orientation = origCamRot * orientation;
      camTransform->SetRotationFromQuaternion(orientation);
	    Vector3 eyePos = Convert_OVRVector3f(eyeRenderPose[eye].Position);
      eyePos.z *= -1; // Need to negate beacuse we use LHS
	  
      // Stationary - Doesn't take into account world rotation
      Vector3 upVec = orientation.RotatedVector(Vector3::cYAxis);
      Vector3 forwardVec = orientation.RotatedVector(Vector3::cZAxis);
      mEyePos[eye] = camTransform->GetGlobalPosition() + eyePos;
	  
	    mView[eye] = Math::LookAtLH(mEyePos[eye], mEyePos[eye] + forwardVec, upVec);
	    camTransform->SetPosition(mEyePos[eye]);

      // Calculates new frustum and assigns oculus view and proj matrix
      vrCam->ComputeOculusFrustum(mView[eye], mProj[eye], mOrthoProj[eye]);

      pRender->RenderScene(vrCam);

	    // Set viewport to left/right eye and render
      pRender->SetBlendState(CRenderer::NO_BLEND_DEPTH_LESS);

      Rect vp = Convert_OVRRecti(OVR::Recti(mEyeRenderViewport[eye]));
      pRender->FinalImageProcessing(vrCam->GetRenderTarget(), &mVR_RT, &vp);

	    camTransform->SetPosition(origCamPos);
      //camTransform->SetRotation(origCamRot);

      pRender->Clear();
	    pRender->ClearRenderQueues();
    }

    ovrHmd_EndFrame(mOvrHmd, eyeRenderPose, &mEyeTexture[0].Texture);


  //mOculusTaskDesc = barrista().togo->Invoke(mOculusTask, MAKEMETAMETHODVARIABLE(RenderOculusTask, RenderOculus), 0);
}

void OculusVR::Resize()
{
  if (mOvrHmd)
  {
    Renderer* pRender = Renderer::getInstance();

    ovrD3D11Config d3d11cfg;
    d3d11cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
    d3d11cfg.D3D11.Header.RTSize = OVR::Sizei(mOvrHmd->Resolution.w, mOvrHmd->Resolution.h);
    d3d11cfg.D3D11.Header.Multisample = 1;
    d3d11cfg.D3D11.pDevice = pRender->md3dDevice;
    d3d11cfg.D3D11.pDeviceContext = pRender->md3dImmediateContext;
    d3d11cfg.D3D11.pBackBufferRT = pRender->mVRFinalRT.GetRenderTargetView();
    d3d11cfg.D3D11.pSwapChain = pRender->mOculusSwapChain;

    ovrFovPort eyeFov[2] = { mOvrHmd->DefaultEyeFov[0], mOvrHmd->DefaultEyeFov[1] };

    ovrBool res = ovrHmd_ConfigureRendering(mOvrHmd, &d3d11cfg.Config,
      ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette |
      ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive,
      eyeFov, mEyeRenderDesc);
  }  
}

bool OculusVR::IsConnected()
{
	if (mOvrHmd)
		return true;

	return false;
}

void OculusVR::SafetyWarning()
{
  ovrHSWDisplayState hswDisplayState;
  ovrHmd_GetHSWDisplayState(mOvrHmd, &hswDisplayState);
  ovrHmd_DismissHSWDisplay(mOvrHmd);
  
  if (hswDisplayState.Displayed)
  {
    // Dismiss the warning if the user pressed the appropriate key or if the user
    // is tapping the side of the HMD.
    // If the user has requested to dismiss the warning via keyboard or controller input...
    if (Input::GetTriggered(0, "Space") == 1.f)
      ovrHmd_DismissHSWDisplay(mOvrHmd);
    else
    {
      // Detect a moderate tap on the side of the HMD.
      ovrTrackingState ts = ovrHmd_GetTrackingState(mOvrHmd, ovr_GetTimeInSeconds());
      if (ts.StatusFlags & ovrStatus_OrientationTracked)
      {
        const OVR::Vector3f v(ts.RawSensorData.Accelerometer.x,
          ts.RawSensorData.Accelerometer.y,
          ts.RawSensorData.Accelerometer.z);
        // Arbitrary value and representing moderate tap on the side of the DK2 Rift.
        if (v.LengthSq() > 250.f)
          ovrHmd_DismissHSWDisplay(mOvrHmd);
      }
    }
  }
}

OVR::Matrix4f OculusVR::CalculateViewFromPose(const OVR::Posef& pose, CTransform* transform)
{
  OVR::Quatf baseQ = OVR::Quatf(OVR::Vector3f(0, 1, 0), DegToRad(transform->rotation.Angles.y));

  OVR::Vector3f BodyPos;
  BodyPos.x = transform->position().x;
  BodyPos.y = transform->position().y;
  BodyPos.z = transform->position().z;

  OVR::Posef worldPose = OVR::Posef(baseQ * pose.Rotation,
                                    BodyPos + baseQ.Rotate(pose.Translation));

  // Rotate and position View Camera
  OVR::Vector3f up = worldPose.Rotation.Rotate(OVR::Vector3f(0, 1, 0));
  OVR::Vector3f forward = worldPose.Rotation.Rotate(OVR::Vector3f(0, 0, 1));

  // Transform the position of the center eye in the real world (i.e. sitting in your chair)
  // into the frame of the player's virtual body.

  //OVR::Vector3f viewPos = ForceZeroHeadMovement ? ThePlayer.BodyPos : worldPose.Translation;
  OVR::Vector3f viewPos = worldPose.Translation;

  OVR::Matrix4f view = OVR::Matrix4f::LookAtLH(viewPos, viewPos + forward, up);
  
  return view;
}

OculusVR* OculusVR::getInstance()
{
  static OculusVR* instance = 0;

  if (!instance)
    instance = new OculusVR();

  return instance;
}

//unsigned OculusVR::GetRecommendedWidth()
//{
//  return mRecommendedSize.w;
//}
//
//unsigned OculusVR::GetRecommendedHeight()
//{
//  return mRecommendedSize.h;
//}

Math::Quaternion OculusVR::Convert_OVRQuat(const OVR::Quat<float>& q)
{
  return Quaternion(q.x, q.y, q.z, q.w);
}

Vector3 OculusVR::Convert_OVRVector3f(const OVR::Vector3f& v)
{
  return Vector3(v.x, v.y, v.z);
}

Rect OculusVR::Convert_OVRRecti(const OVR::Recti& r)
{
  return Rect((float)r.x, (float)r.y, (float)r.w, (float)r.h);
}

OculusVR::~OculusVR()
{
  Release();
}

void OculusVR::Release()
{
  ovrHmd_Destroy(mOvrHmd);
  mOvrHmd = 0;

  // No OVR functions involving memory are allowed after this.
  ovr_Shutdown();
}

//Vector3 OculusVR::GetEyePos(unsigned index)
//{
//  return mEyePos[index];
//}
//
//Math::Vector3 OculusVR::GetEyePos()
//{
//  return mEyePos[mCurrentEyeIndex];
//}
//
//Math::Matrix4 OculusVR::GetProjMatrix(unsigned index)
//{
//  return mProj[index];
//}
//
//Math::Matrix4 OculusVR::GetViewMatrix(unsigned index)
//{
//  return mView[index];
//}

ovrHmd OculusVR::GetOvrHmd() const
{
  return mOvrHmd;
}

//ovrTexture* OculusVR::GetOvrTexture()
//{
//  return &mEyeTexture[0].Texture;
//}
//
//ovrPosef* OculusVR::GetOvrEyeRenderPos()
//{
//  return mEyeRenderPose;
//}

