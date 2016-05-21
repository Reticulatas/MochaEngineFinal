#include "stdafx.h"
#include "SteamViveVR.h"
#include "Renderer.h"
#include "Log.h"
#include <direct.h>

SteamVR::SteamVR()
{
  mVRHmd = 0;
}

SteamVR::~SteamVR()
{
  Release();
}

bool SteamVR::Init()
{
  if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
  {
    // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
    // local Steam client and also launches this game again.

    // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
    // removed steam_appid.txt from the game depot.
    return false;
  }

  if (!SteamAPI_Init())
  {
    Log("SteamAPI_Init() failed\n");
    return false;
  }

  // set our debug handler
  //SteamClient()->SetWarningMessageHook(&SteamAPIDebugTextHook);

  // Ensure that the user has logged into Steam. This will always return true if the game is launched
  // from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
  // will return false.
  if (!SteamUser()->BLoggedOn())
  {
    Log("Steam user is not logged in\n");
    Log("Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n");
    return false;
  }

  // Initialize steam controller if connected
  char rgchCWD[1024];
  _getcwd(rgchCWD, sizeof(rgchCWD));

  char rgchFullPath[1024];
  _snprintf(rgchFullPath, sizeof(rgchFullPath), "%s\\%s", rgchCWD, "controller.vdf");

  if (!SteamController()->Init(rgchFullPath))
  {
    Log("SteamController()->Init() failed\n");
    Log("Steam Controller Init failed. Is controller.vdf in the current working directory?\n");
    return false;
  }

  // Need to run from Steam UI to work I think
  bool UseVR = SteamUtils()->IsSteamRunningInVR();
  // Set UseVR to true for testing
  UseVR = true;

  // Init VR
  if (!UseVR)
  {
    mVRHmd = NULL;
  }
  else
  {
    vr::HmdError error;
    mVRHmd = vr::VR_Init(&error);
    if (!mVRHmd)
    {
      Log("Failed to initialize VR.");
      Log("Game will run without VR.");
      return false;
    }
  }

  Log("SteamVR Initialized");
  return true;
}

bool SteamVR::StartUp()
{
  if (!mVRHmd)
    return false;

  mVRHmd->AttachToWindow(Engine::getInstance()->GetVRWnd()->GetHWND());

  // Get recommended width and height of textures
  uint32 w, h;
  mVRHmd->GetRecommendedRenderTargetSize(&w, &h);
  mRecommendedWidth = w;
  mRecommendedHeight = h;

  mFinalVR_VP.width = (float)Engine::getInstance()->GetVRWnd()->GetWindowWidth();
  mFinalVR_VP.height = (float)Engine::getInstance()->GetVRWnd()->GetWindowHeight();

  mVR_RT.InitRenderTarget(mRecommendedWidth, mRecommendedHeight, Renderer::getInstance()->GetFrameBufferFormat());

  Renderer::getInstance()->OnResize();

  // Create distortion textures
  CreateDistortionTexture();

  // Compute Proj matrices - Note different from oculus proj matrix, may cause problems in the future
  for (int i = 0; i < EyeCount; ++i)
  {
    vr::Hmd_Eye eye = (vr::Hmd_Eye)i;
    // Matrix is exactly the same with DirectX and OpenGL...
    vr::HmdMatrix44_t matProjection = mVRHmd->GetProjectionMatrix(eye, 0.1f, 1000.f, vr::API_DirectX);
    Matrix4 matProj(&matProjection.m[0][0]);

    // Need to negate z - API_DirectX not LHS
    mProj[i] = matProj;
    mProj[i].m02 *= -1;
    mProj[i].m12 *= -1;
    mProj[i].m22 *= -1;
    mProj[i].m32 *= -1;

    // Copied orthographics proj calculation from oculus sdk
    vr::HmdMatrix34_t matEye = mVRHmd->GetHeadFromEyePose(eye);
    float orthoDistance = .8f; // 2D is 0.8 meter from camera
    float PixelsPerTanAngleAtCenter = 549.618286f;
    Vector2 orthoScale = Vector2(1, 1) / Vector2(PixelsPerTanAngleAtCenter, PixelsPerTanAngleAtCenter);
    mOrthoProj[i] = OrthographicLH_VR(mProj[i], orthoScale, orthoDistance, -matEye.m[0][3]);
  }

  return true;
}

void SteamVR::Release()
{
  if (mVRHmd)
  {
    vr::VR_Shutdown();
    mVRHmd = NULL;
  }
}

bool SteamVR::IsConnected()
{
  if(mVRHmd)
    return true;
  return false;
}

void SteamVR::RenderVR(CCamera* vrCam)
{
  // Run Steam client callbacks
  SteamAPI_RunCallbacks();

  Renderer* pRender = Renderer::getInstance();

  // Must clear to black
  pRender->ClearFrameBuffer(mVR_RT.GetRenderTargetView(), COLOR::BLACK);
  Vector3 origCamPos = vrCam->gameObject->transform->GetGlobalPosition();

  // Scene render - left / right eyes
  for (int i = 0; i < EyeCount; ++i)
  {
    vr::Hmd_Eye eye = (vr::Hmd_Eye)i;
    mCurrentEyeIndex = eye;

    // Update eye pos and view matrix based on head tracker
    UpdateCamFromTracker(vrCam, eye);
    
    // Render the scene
    pRender->RenderScene(vrCam);
    
    // Set viewport to left/right eye and render
    pRender->SetBlendState(CRenderer::NO_BLEND_DEPTH_LESS);

    // Tone Mapping / Gamma correction
    pRender->FinalImageProcessing(vrCam->GetRenderTarget(), &mVR_RT);

    // Distortion render
    GetViewportFromHmd(eye, mFinalVR_VP);
    pRender->SetViewport(mFinalVR_VP);
    pRender->PostProcess(&mVR_RT, &mDistortionRT[i], &pRender->mVRFinalRT, Shader::DISTORTION_RENDER, false);
    
    // Set back original head position
    vrCam->gameObject->transform->SetPosition(origCamPos);

    // End Scene
    pRender->Clear();
    pRender->ClearRenderQueues();
  }

  // Swap buffers
  if (mVsyncEnabled)
    pRender->GetOculusSwapChain()->Present(1, 0);
  else
    pRender->GetOculusSwapChain()->Present(0, 0);
}

vr::IHmd* SteamVR::GetHMD() const
{
  return mVRHmd;
}

SteamVR* SteamVR::getInstance()
{
  static SteamVR* instance = 0;

  if (!instance)
    instance = new SteamVR();

  return instance;
}

void SteamVR::CreateDistortionTexture()
{
  int nDistSize = 128;
  int elem_span = 4;

  struct DistortionSample_t
  {
    uint16 redU;
    uint16 redV;
    uint16 blueU;
    uint16 blueV;
  };

  for (int i = 0; i < EyeCount; i++)
  {
    vr::Hmd_Eye eye = (vr::Hmd_Eye)i;

    //uint16 *pData = (uint16 *)malloc(sizeof(uint16)* elem_span * nDistSize * nDistSize);
    uint16 *pData = new uint16[elem_span * nDistSize * nDistSize];

    for (int yi = 0; yi < nDistSize; yi++)
    {
      const float v = (yi + 0.5f) / (float)nDistSize;
      for (int xi = 0; xi < nDistSize; xi++)
      {
        const float u = (xi + 0.5f) / (float)nDistSize;

        vr::DistortionCoordinates_t coords = mVRHmd->ComputeDistortion(eye, u, v);
        Vector2 samp_red(coords.rfRed);
        Vector2 samp_green(coords.rfGreen);
        Vector2 samp_blue(coords.rfBlue);

        static const float rg_to_rb_ratio = 0.522f;
        Vector2 red_to_blue = samp_blue - samp_red;
        Vector2 tex_samp_red = samp_green - rg_to_rb_ratio * red_to_blue;
        Vector2 tex_samp_blue = samp_green + (1 - rg_to_rb_ratio) * red_to_blue;

        if (tex_samp_red.x < 0.0 || tex_samp_blue.x < 0.0)
        {
          tex_samp_red.x = tex_samp_blue.x = 0.0;
        }

        if (tex_samp_red.x > 1.0 || tex_samp_blue.x > 1.0)
        {
          tex_samp_red.x = tex_samp_blue.x = 1.0;
        }

        if (tex_samp_red.y < 0.0 || tex_samp_blue.y < 0.0)
        {
          tex_samp_red.y = tex_samp_blue.y = 0.0;
        }

        if (tex_samp_red.y > 1.0 || tex_samp_blue.y > 1.0)
        {
          tex_samp_red.y = tex_samp_blue.y = 1.0;
        }

        int idx = yi * nDistSize + xi;

        pData[idx * elem_span + 0] = (uint16)(tex_samp_red.x * 65535.f);
        pData[idx * elem_span + 1] = (uint16)(tex_samp_red.y * 65535.f);
        pData[idx * elem_span + 2] = (uint16)(tex_samp_blue.x * 65535.f);
        pData[idx * elem_span + 3] = (uint16)(tex_samp_blue.y * 65535.f);
      }
    }

    bool result = mDistortionRT[i].InitRenderTargetFromMemory(nDistSize, nDistSize, DXGI_FORMAT_R16G16B16A16_UNORM, pData);
    delete [] pData;
    if (!result)
    {
      Log("Unable to create distortion texture\n");
      return;
    }
  }
}

void SteamVR::UpdateCamFromTracker(CCamera* cam, int eye)
{
  vr::HmdMatrix34_t pose;
  vr::HmdTrackingResult eResult;
  mVRHmd->GetTrackerFromHeadPose(0, &pose, &eResult);

  Matrix3 rot(pose.m[0][0], pose.m[0][1], pose.m[0][2],
              pose.m[1][0], pose.m[1][1], pose.m[1][2],
              pose.m[2][0], pose.m[2][1], pose.m[2][2]);
  Vector3 eyePos(pose.m[0][3], pose.m[1][3], pose.m[2][3]);
  eyePos.z *= -1;

  // Need to negate z - LHS
  Quaternion orientation = ToQuaternion(rot);
  orientation.Invert();
  orientation.z *= -1;

  Vector3 upVec = orientation.RotatedVector(Vector3::cYAxis);
  Vector3 forwardVec = orientation.RotatedVector(Vector3::cZAxis);
  CTransform* camTransform = cam->gameObject->transform;
  mEyePos[eye] = camTransform->GetGlobalPosition() + eyePos;

  mView[eye] = Math::LookAtLH(mEyePos[eye], mEyePos[eye] + forwardVec, upVec);
  camTransform->SetPosition(mEyePos[eye]);
  camTransform->SetRotationFromQuaternion(orientation);

  // Calculates new frustum and assigns oculus view and proj matrix
  cam->ComputeOculusFrustum(mView[eye], mProj[eye], mOrthoProj[eye]);
}

void SteamVR::GetViewportFromHmd(int eye, Rect& r)
{
  // get the viewport from the Hmd
  unsigned vpx, vpy, vpw, vph;
  mVRHmd->GetEyeOutputViewport((vr::Hmd_Eye)eye, &vpx, &vpy, &vpw, &vph);
  r.x = (float)vpx;
  r.y = (float)vpy;
  r.width = (float)vpw;
  r.height = (float)vph;
}
