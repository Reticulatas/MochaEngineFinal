#include "stdafx.h"
#include "RenderTasks.h"
#include "EditorSubsystem.h"
#include "CForwardRenderer.h"
#include "CDiffuseRenderer.h"
#include "ThreadedQueue.h"
#include "CFontRenderer.h"
#include <chrono>
#include "Oculus.h"

FrustumCullingTask::FrustumCullingTask() : Task()
{

}

FrustumCullingTask::~FrustumCullingTask()
{

}

bool FrustumCullingTask::Tick()
{
	const std::list<CCamera*>* camList;

	ThreadedQueue tqueue;

	if (Engine::getInstance()->GetIsEditorMode() && !EditorSubsystem::getInstance()->IsTesting())
		camList = EditorSubsystem::getInstance()->GetEditorState()->GetListOfComponentsOfType<CCamera>();
	else
		camList = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CCamera>();

	BarristaToGo::BarristaToGoDescriptor* descriptor = 0;
	std::vector<ThreadQueueNode*> blocks;
	for (auto& camIter : *camList)
	{
		void* cam = camIter;
		meta::metafunction_method* mt = MAKEMETAMETHODVARIABLE(FrustumCullingTask, FrustumCullToGo);
		mt->prepack_params(cam);
		blocks.push_back(tqueue.GetStart()->CallAfter(mt, this));
		FrustumCullToGo(camIter);
	}
	blocks.front()->JoinBlockAndEnd(blocks);
	//tqueue.Execute();

	for (auto& cam : *camList) {
		cam->mVisibleObjects = Renderer::getInstance()->mMapDeferredQueues.size() + Renderer::getInstance()->mMapLightQueues.size();
	}

	return true;
}

void FrustumCullingTask::FrustumCullToGo(CCamera* cam)
{
	FrustrumCull(cam, GSM::getInstance()->getActiveState());
	if (Engine::getInstance()->GetIsEditorMode() && !EditorSS::IsTesting())
		FrustrumCull(cam, EditorSubsystem::getInstance()->GetEditorState());
}

void FrustumCullingTask::FrustrumCull(CCamera* cam, BaseState* state)
{
	Renderer* renderer = Renderer::getInstance();

	// Points of the AABB in world space
	std::vector<Vector3> verts(8);
	Vector3 origin, scaledHalfVec, rotatedX, rotatedY, rotatedZ;
	Quaternion rot;

  std::vector<CRenderer*>* deferredQueue;
  std::vector<CRenderer*>* forwardQueue;
  std::vector<CRenderer*>* diffuseQueue;
  std::vector<CRenderer*>* fontQueue;
	std::vector<CLight*>* lightQueue;

	//the only thread sensitive area should be here
	{
		ThreadUtils::ThreadLockVisitor tlv(4);
		deferredQueue = &renderer->mMapDeferredQueues[cam];
		forwardQueue = &renderer->mMapForwardQueues[cam];
		diffuseQueue = &renderer->mMapDiffuseQueues[cam];
   fontQueue = &renderer->mMapFontQueues[cam];
		lightQueue = &renderer->mMapLightQueues[cam];
	}

	const std::list<CRenderer*>* listRenderers = state->GetListOfComponentsOfType<CRenderer>();

	if (listRenderers)
	{
		// Get vertices in world space
		for (auto& iter : *listRenderers)
		{
			if (!iter->isEnabled() || !iter->mMesh.Valid())
				continue;

			origin = iter->GetMesh()->GetOrigin();
			rot = iter->gameObject->transform->GetRotationQuaternion();
			scaledHalfVec = iter->GetMesh()->GetAABB().maxPoint - origin;
			scaledHalfVec *= iter->gameObject->transform->scale;
			origin *= iter->gameObject->transform->scale;

			rotatedX = rot.RotatedVector(Vector3::cXAxis * scaledHalfVec.x);
			rotatedY = rot.RotatedVector(Vector3::cYAxis * scaledHalfVec.y);
			rotatedZ = rot.RotatedVector(Vector3::cZAxis * scaledHalfVec.z);
			rot.RotateVector(&origin);
			origin += iter->gameObject->transform->GetGlobalPosition();

			// + + +
			verts[0] = origin + rotatedX + rotatedY + rotatedZ;
			// + + -
			verts[1] = origin + rotatedX + rotatedY - rotatedZ;
			// + - +
			verts[2] = origin + rotatedX - rotatedY + rotatedZ;
			// - + +
			verts[3] = origin - rotatedX + rotatedY + rotatedZ;
			// - - +
			verts[4] = origin - rotatedX - rotatedY + rotatedZ;
			// - + -
			verts[5] = origin - rotatedX + rotatedY - rotatedZ;
			// + - -
			verts[6] = origin + rotatedX - rotatedY - rotatedZ;
			// - - -
			verts[7] = origin - rotatedX - rotatedY - rotatedZ;

			// AABB cull check
			if (cam->CullAABB(verts))
			{
        if (iter->mymeta() == CMeshRenderer::getmeta() || iter->mymeta() == CSkinMeshRenderer::getmeta())
          deferredQueue->push_back(iter);

				else if (iter->mymeta() == CForwardRenderer::getmeta())
          forwardQueue->push_back(iter);

        else if (iter->mymeta() == CDiffuseRenderer::getmeta())
          diffuseQueue->push_back(iter);

        else if (iter->mymeta() == CFontRenderer::getmeta())
          fontQueue->push_back(iter);
			}
		}
	}

	const std::list<CLight*>* listLight = state->GetListOfComponentsOfType<CLight>();
	if (listLight)
	{
		// Iterate through all Lights
		for (auto& light : *listLight)
		{
			if (!light->isEnabled())
				continue;


			if (light->mLightType == CLight::DIRECTIONAL)
			{
				lightQueue->push_back(light);
				continue;
			}

			Vector3 lightPos = light->gameObject->transform->GetGlobalPosition();

			if (cam->CullSphere(lightPos, light->GetRange()))
				lightQueue->push_back(light);
		}
  }  
}

FrustumCullingTask* FrustumCullingTask::getInstance()
{
	static FrustumCullingTask* instance = 0;

	if (!instance)
		instance = new FrustumCullingTask();

	return instance;
}

meta_define(FrustumCullingTask);
meta_define(RenderOculusTask);

bool RenderOculusTask::Tick()
{
  //not used
  RenderOculus();

  return true;
}

void RenderOculusTask::RenderOculus()
{  
  //ovrHmd_BeginFrame(OculusVR::getInstance()->GetOvrHmd(), 0);
  //
  //promise.set_value();
  //
  //{
  //  ThreadUtils::ThreadLockVisitor tv(6);
  //
  //  ovrHmd_EndFrame(OculusVR::getInstance()->GetOvrHmd(), OculusVR::getInstance()->GetOvrEyeRenderPos(), OculusVR::getInstance()->GetOvrTexture());
  //
  //  Renderer::getInstance()->CopyCamRenderTarget(*oculusRT, oculusCam);
  //
  //  if (Renderer::getInstance()->IsVSyncEnabled())
  //    Renderer::getInstance()->GetOculusSwapChain()->Present(1, 0);
  //  else
  //    Renderer::getInstance()->GetOculusSwapChain()->Present(0, 0);
  //}
}
