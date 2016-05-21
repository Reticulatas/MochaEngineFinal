/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CReflectionProbe.h"

void CReflectionProbe::OnFree(void)
{
	if (mReflectionRT)
	{
		delete mReflectionRT;
		mReflectionRT = 0;
	}
}

void CReflectionProbe::OnInit(void)
{
 CCamera::OnInit();
 
  myType = "CReflectionProbe";

  mReflectionRT = 0;
  mCubemapWidth = 1024;
  mStatic = true;
  mRefreshProbe = true;
  mProbeBoxDim = Vector3(1.0f);
}

void CReflectionProbe::OnStart(void)
{
	CCamera::OnStart();

	mReflectionRT = new RenderTarget(true);
	mReflectionRT->InitRenderTargetCube(mCubemapWidth, mCubemapWidth, Renderer::getInstance()->GetFrameBufferFormat(), 1.0f, 0);
	//Resize();
	//SetPerspectiveProj(Engine::getInstance()->GetGameWnd()->FixedAspectRatio(), cPi * .5f);
	//SetPerspectiveProj(Engine::getInstance()->GetGameWnd()->FixedAspectRatio(), cPi * .5f / Engine::getInstance()->GetGameWnd()->FixedAspectRatio());
	SetPerspectiveProj(1.0f, mNearZ, mFarZ,cPi * .5f);
	mRefreshProbe = true;
}

void CReflectionProbe::OnUpdate(void)
{
	if (!mStatic && !mRefreshProbe)
	{
		mRefreshProbe = true;
	}
}

void CReflectionProbe::OnEditorUpdate(void)
{
	OnUpdate();
}

CReflectionProbe::CReflectionProbe() : CCamera()
{
  
}

CReflectionProbe::~CReflectionProbe()
{

}

RenderTarget* CReflectionProbe::GetRenderTarget(void) const
{
	return mReflectionRT;
}

bool CReflectionProbe::ShouldRefresh(void)
{
	return mRefreshProbe;
}

void CReflectionProbe::OnRefreshed(void)
{
	mRefreshProbe = false;
}

const Vector3& CReflectionProbe::GetBoxDimentions(void)
{
	return mProbeBoxDim;
}

void CReflectionProbe::SetupCameraDirection(unsigned cubemapDirectionIndex /*= 0*/)
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
	CCamera::UpdateViewMatrix();
	CCamera::ComputeFrustum();
	mReflectionRT->SetActiveRenderTargetView(cubemapDirectionIndex);
}

void CReflectionProbe::OnDraw()
{
	if (gameObject->mIsSelected)
	{
		Vector3& probePos = gameObject->transform->position();

		Math::Vector3 pos0 = probePos + mProbeBoxDim;
		Math::Vector3 pos1 = probePos + Vector3(-mProbeBoxDim.x, mProbeBoxDim.y, mProbeBoxDim.z);
		Math::Vector3 pos2 = probePos + Vector3(-mProbeBoxDim.x, -mProbeBoxDim.y, mProbeBoxDim.z);
		Math::Vector3 pos3 = probePos + Vector3(mProbeBoxDim.x, -mProbeBoxDim.y, mProbeBoxDim.z);

		Math::Vector3 pos4 = probePos + Vector3(mProbeBoxDim.x, -mProbeBoxDim.y, -mProbeBoxDim.z);
		Math::Vector3 pos5 = probePos + Vector3(-mProbeBoxDim.x, -mProbeBoxDim.y, -mProbeBoxDim.z);
		Math::Vector3 pos6 = probePos + Vector3(-mProbeBoxDim.x, mProbeBoxDim.y, -mProbeBoxDim.z);
		Math::Vector3 pos7 = probePos + Vector3(mProbeBoxDim.x, mProbeBoxDim.y, -mProbeBoxDim.z);


		Renderer::getInstance()->DrawLinePerma(pos0, pos1, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos1, pos2, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos2, pos3, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos3, pos0, COLOR::RED);

		Renderer::getInstance()->DrawLinePerma(pos4, pos5, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos5, pos6, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos6, pos7, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos7, pos4, COLOR::RED);

		Renderer::getInstance()->DrawLinePerma(pos0, pos7, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos3, pos4, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos2, pos5, COLOR::RED);
		Renderer::getInstance()->DrawLinePerma(pos6, pos1, COLOR::RED);
	}
}

void CReflectionProbe::Resize()
{
	//SetPerspectiveProj(Engine::getInstance()->GetGameWnd()->FixedAspectRatio(), cPi * .5f);
	//mReflectionRT->Resize();
	//mRefreshProbe = true;
}

meta_define(CReflectionProbe);
___COMP_REGISTER(CReflectionProbe);
/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change
