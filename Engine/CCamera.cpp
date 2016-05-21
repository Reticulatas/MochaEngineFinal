/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */

#include "stdafx.h"
#include "CCamera.h"
#include "CTransform.h"
#include "TextureNode.h"
#include "ObjectFactory.h"
#include "EditorSubsystem.h"

void CCamera::OnInit(void)
{
  mCamType = GENERIC;
  mActiveProj = PERSP;
  mPreviousProj = UNASSIGNED;
  mFoVmodifier = 1.f;
  mVisibleObjects = 0;
  DispatchClickMessages = false;
  mNearZ = .1f;
  mFarZ = 1000.f;

  //note: moved computefrustrum to onstart

  mCameraRT = AssetManager::getInstance()->GetAsset("DefaultRT.mrt");
}

void CCamera::OnStart(void)
{
	Component::OnStart();

	SetProjection(Engine::getInstance()->GetGameWnd()->FixedAspectRatio());
	Resize();
	ComputeFrustum();
}

void CCamera::OnUpdate(void)
{
  if (mActiveProj != mPreviousProj || mCamType == GENERIC)
    SetProjection(Engine::getInstance()->GetGameWnd()->FixedAspectRatio());

  if (mCamType != VR || !Renderer::getInstance()->GetVR()->IsConnected())
  {
    UpdateViewMatrix();
    ComputeFrustum();
  }

  if (DispatchClickMessages)
  {
	  //camera dispatches OnClick() events to components
	  if (Engine::getInstance()->GetIsEditorMode() && EditorSubsystem::getInstance()->EditorCamera() == this)
	  {
      GameObject* selected = PhysicEngine::getInstance()->RayCast3D(this->gameObject->transform->position(),
                                                this->gameObject->transform->GetForwardVector(),
                                                this, Input::GetMouseX(true), Input::GetMouseY(true), 10000.0f);
      
      if (selected)
      {
        selected->OnMouseEnter(this);
        if (Input::GetTriggered(0, "MouseLClick") == 1.0f)
          selected->OnMouseClick(this);
      }
	  }
  }
}

void CCamera::OnFree(void)
{

}

CCamera::CCamera(void) : Component()
{	
}	

Vector3 CCamera::GetPosition() const
{
  return gameObject->transform->position();
}

void CCamera::SetPosition(const float x, const float y, const float z)
{
	gameObject->transform->SetPosition(Vector3(x, y, z));
}

float CCamera::GetNearZ() const
{
  return mNearZ;
}

float CCamera::GetFarZ() const
{
  return mFarZ;
}

float CCamera::GetAspect() const
{
  return mAspect;
}

float CCamera::GetFovY() const
{
  return mFovY;
}

float CCamera::GetFovX() const
{
  float halfWidth = 0.5f * GetNearWindowWidth();
  return 2.0f*atan(halfWidth / mNearZ);
}

float CCamera::GetNearWindowWidth() const
{
  return mAspect * mNearWindowHeight;
}

float CCamera::GetNearWindowHeight() const
{
  return mNearWindowHeight;
}

float CCamera::GetFarWindowWidth() const
{
  return mAspect * mFarWindowHeight;
}

float CCamera::GetFarWindowHeight() const
{
  return mFarWindowHeight;
}

Matrix4 CCamera::View() const
{
  return mView;
}

Matrix4 CCamera::Proj() const
{
  return mProj;
}

Matrix4 CCamera::ProjView() const
{
  return mProj * mView;
}

void CCamera::Strafe(float dt)
{
	gameObject->transform->Translate(gameObject->transform->GetRightVector() * dt);
}

void CCamera::Walk(float dt)
{
	gameObject->transform->Translate(gameObject->transform->GetForwardVector() * dt);
}

void CCamera::Pitch(float angle)
{
  // Concatenate new rotation with current rotation
  Quaternion q = ToQuaternion(gameObject->transform->GetRightVector(), angle);
  q *= gameObject->transform->GetRotationQuaternion();
  gameObject->transform->SetRotation(q);
}

void CCamera::RotateY(float angle)
{
  // Concatenate new rotation with current rotation
  Quaternion q = ToQuaternion(Vector3::cYAxis, angle);
  q *= gameObject->transform->GetRotationQuaternion();
  gameObject->transform->SetRotation(q);
}

void CCamera::UpdateViewMatrix()
{
  Vector3 mUp = gameObject->transform->GetUpVector();
  Vector3 mLook = gameObject->transform->GetForwardVector();

  Vector3 pos = gameObject->transform->GetGlobalPosition();
  mView = Math::LookAtLH(pos, pos + mLook, mUp);
}

void CCamera::SetPerspectiveProj(float aspect, float znear, float zfar, float fovY)
{
  // Store properties
  mFovY = fovY * mFoVmodifier;
  mAspect = aspect;

  // Calculate near plane height and far plane height
  mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f* mFovY );
  mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f * mFovY );

  //XMMATRIX Proj = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
  //mProj = Matrix4(&Proj.m[0][0]);

  // Need to transpose because of the order we send into the shader
  //mProj.Transpose();

  mProj = PerspectiveLH(mFovY, mAspect, mNearZ, mFarZ);
}

void CCamera::SetOrthographicProj(float aspect, float znear, float zfar, float fovY)
{
  // Store properties
  mFovY = fovY;
  mAspect = aspect;

  // Calculate near plane height and far plane height
  //mNearWindowHeight = 2.0f * mNearZ * mFoVmodifier;
  //mFarWindowHeight = mNearWindowHeight;
  mNearWindowHeight = mFarWindowHeight = (float)Engine::getInstance()->GetGameWnd()->GetWindowHeight();
  mNearWindowHeight = mFarWindowHeight = 9 * mFoVmodifier;

  //XMMATRIX Proj = XMMatrixOrthographicLH((float)Engine::getInstance()->GetGameWnd()->GetWindowWidth(), (float)Engine::getInstance()->GetGameWnd()->GetWindowHeight(), mNearZ, mFarZ);
  //mProj = Matrix4(&Proj.m[0][0]);

  mProj = OrthographicLH(GetNearWindowWidth(), mNearWindowHeight, mNearZ, mFarZ);
}

void CCamera::SetProjection(float aspect)
{
  mPreviousProj = mActiveProj;

  switch (mActiveProj)
  {
  default:
  case CCamera::PERSP:
    SetPerspectiveProj(aspect, mNearZ, mFarZ);
    break;
  case CCamera::ORTHO:
    SetOrthographicProj(aspect, mNearZ, mFarZ);
    break;
  }

  UpdatePostProcessMatrix();
}

Vector3 CCamera::ScreenToWorldPosition(int x, int y)
{
  MochaWindow* mWnd = Engine::getInstance()->GetFocusedWnd();

  Vector3 rayDir;

  float windowWidth = (float)mWnd->GetWindowWidth();
  float windowHeight = (float)mWnd->GetWindowHeight();

  if (Engine::getInstance()->GetIsLetterBox() && mCamType == MAIN)
  {
    windowWidth = (float)mWnd->GetViewportWidth();
    windowHeight = (float)mWnd->GetViewportHeight();
    x -= (int)mWnd->GetScreenOffsetX();
    y -= (int)mWnd->GetScreenOffsetY();
  }

  rayDir.x = ((+2.0f * (float)x / windowWidth) - 1.0f) / mProj(0, 0);
  rayDir.y = ((-2.0f * (float)y / windowHeight) + 1.0f) / mProj(1, 1);
  rayDir.z = 1;

  rayDir = TransformNormal(mView.Inverted(), rayDir);
  rayDir.Normalize();
  
  return rayDir;
}

void CCamera::ComputeFrustum()
{
  mFrustum.SetPoints(this);
  mFrustum.ComputeFrustumPlane(mView.Transposed() * mProj.Transposed());
}

bool CCamera::CullSphere(const Vector3& pt, float range)
{
  return mFrustum.CircleInsideFrustum(pt, range);
}

void CCamera::Resize()
{
  if (mCameraRT.Valid())
    mCameraRT.GetAssetPtr<RenderTarget>()->Resize();

  mOrthoProj = OrthographicLH((float)Engine::getInstance()->GetGameWnd()->GetWindowWidth(), (float)Engine::getInstance()->GetGameWnd()->GetWindowHeight(), -1, mFarZ);
}

void CCamera::UpdatePostProcessMatrix()
{
  Matrix4 world;
  world(0, 0) = GetNearWindowWidth();
  world(1, 1) = mNearWindowHeight;

  //-1 to 1 for near and far plane since it is orthographic
  //XMMATRIX p = XMMatrixOrthographicLH(GetNearWindowWidth(), mNearWindowHeight, -1, 1);
  //Matrix4 proj(&p.m[0][0]);

  Matrix4 proj = OrthographicLH(GetNearWindowWidth(), mNearWindowHeight, -1, 1);

  // No need for View transform
  mPostProcess = proj * world;
}

Math::Matrix4 CCamera::PostProcessMatrix() const
{
  return mPostProcess;
}

RenderTarget* CCamera::GetRenderTarget(void) const
{
  return mCameraRT.GetAssetPtr<RenderTarget>();
}

void CCamera::SetRenderTarget(AssetHandle assetH)
{
  mCameraRT = assetH;
}

void CCamera::SetRenderTarget(std::string name)
{
  mCameraRT = AssetManager::getInstance()->GetAsset(name);
}

void CCamera::SetupCameraDirection(unsigned cubemapDirectionIndex /*= 0*/)
{

}

void CCamera::OnRefreshed(void)
{

}

CCamera::CamType CCamera::GetCamType() const
{
  return mCamType;
}

AssetHandle CCamera::GetRenderTargetAssetHandle() const
{
  return mCameraRT;
}

void CCamera::OnDraw(void)
{
  if (gameObject->mIsSelected)
  {
    SetProjection(Engine::getInstance()->GetGameWnd()->FixedAspectRatio());
    ComputeFrustum();
    mFrustum.DrawFrustum();
  }    
}

bool CCamera::CullAABB(const std::vector<Vector3>& box)
{
  return mFrustum.RectInsideFrustum(box);
}

void CCamera::ComputeOculusFrustum(const Matrix4& view, const Matrix4& proj, const Matrix4& ortho)
{
  mView = view;
  mProj = proj;
  mOrthoProj = ortho;

  ComputeFrustum();
}

Math::Matrix4 CCamera::Ortho() const
{
	return mOrthoProj;
}

bool CCamera::IsOculusCam() const
{
	return (mCamType == VR) && Renderer::getInstance()->GetVR()->IsConnected();
}

void CCamera::OnEditorStart(void)
{
	EditorSubsystem::getInstance()->widgetManager.RegisterWidget(gameObject, EditorSubsystemTools::WidgetManager::CAMERA);
}

void CCamera::OnEditorFree(void)
{
	EditorSubsystem::getInstance()->widgetManager.DeRegisterWidget(gameObject);
}

bool CCamera::sortCam::operator()(CCamera* lhs, CCamera* rhs) const
{
  if (lhs->mCamType != rhs->mCamType)
    return lhs->mCamType < rhs->mCamType;
  else
    return lhs->gameObject->GetID() < rhs->gameObject->GetID();
}

___END_REGISTERED___

meta_define(CCamera);
___COMP_REGISTER(CCamera);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change