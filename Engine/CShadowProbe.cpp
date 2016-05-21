/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CShadowProbe.h"

void CShadowProbe::OnFree(void)
{
	if (mShadowMap)
	{
		delete mShadowMap;
		mShadowMap = 0;
	}
}

void CShadowProbe::OnInit(void)
{
 CCamera::OnInit();
 
  myType = "CShadowProbe";

  mShadowMap = 0;
  mCubemapWidth = 1024;
  mStatic = true;
  mRefreshProbe = true;
  mStrength = 1;
  mBias = .001f;
  mUseSoftShadows = false;
  mNearZ = .1f;
  mFarZ = 50;
}

void CShadowProbe::OnStart(void)
{
	CCamera::OnStart();

  mShadowMap = new RenderTarget(true);
  mShadowMap->InitRenderTargetDepth(mCubemapWidth, mCubemapWidth, DXGI_FORMAT_R32_TYPELESS);
	// Set Fov to 90 degrees 
	SetPerspectiveProj(1.0f, mNearZ, mFarZ,cPi * .5f);
	mRefreshProbe = true;

  RequireDependency<CLight>();
  mFarZ = gameObject->GetComponent<CLight>()->GetRange();
}

void CShadowProbe::OnUpdate(void)
{
	if (!mStatic && !mRefreshProbe)
	{
		mRefreshProbe = true;
	}
}

void CShadowProbe::OnEditorUpdate(void)
{
	OnUpdate();
}

CShadowProbe::CShadowProbe() : CCamera()
{
  
}

CShadowProbe::~CShadowProbe()
{

}

RenderTarget* CShadowProbe::GetRenderTarget(void) const
{
	return mShadowMap;
}

bool CShadowProbe::ShouldRefresh(void)
{
	return mRefreshProbe;
}

void CShadowProbe::OnRefreshed(void)
{
	mRefreshProbe = false;
}

void CShadowProbe::SetupCameraDirection(unsigned cubemapDirectionIndex /*= 0*/)
{
	switch (cubemapDirectionIndex)
	{
	case RenderTarget::CUBE_RIGHT:
		gameObject->transform->Rotate(0, 90, 0);
		break;
	case RenderTarget::CUBE_LEFT:
		gameObject->transform->Rotate(0, -90, 0);
		break;
	case RenderTarget::CUBE_UP:
		gameObject->transform->Rotate(-90, 0, 0);
		break;
	case RenderTarget::CUBE_DOWN:
		gameObject->transform->Rotate(90, 0, 0);
		break;
	case RenderTarget::CUBE_FRONT:
		gameObject->transform->Rotate(0, 0, 0);
		break;
	case RenderTarget::CUBE_BACK:
		gameObject->transform->Rotate(0, 180, 0);
		break;
	default:
		return;
		break;
  }

  // Need to update transform matrix for view and frustum calculations
  //gameObject->transform->OnUpdate();

  SetPerspectiveProj(1.0f, mNearZ, mFarZ, cPi * .5f);
	CCamera::UpdateViewMatrix();
	CCamera::ComputeFrustum();
	mShadowMap->SetActiveDepthStencilView(cubemapDirectionIndex);
}

bool CShadowProbe::IsValid()
{
  if (mShadowMap && isEnabled())
    return true;

  return false;
}

void CShadowProbe::OnDraw()
{
}

void CShadowProbe::OnEditorStart(void)
{

}

void CShadowProbe::OnEditorFree(void)
{

}

meta_define(CShadowProbe);
___COMP_REGISTER(CShadowProbe);
/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change
