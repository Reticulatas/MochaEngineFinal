#include "stdafx.h"

#include "CAnimationController.h"
#include "CSkinMeshRenderer.h"
#include "FrameController.h"
#include "GameObject.h"
#include "CTransform.h"
#include "InputDeviceType.h"
#include "InputManager.h"
#include "PlayerInput.h"
#include "XboxButtonTypes.h"
#include "XboxInputDevice.h"

CAnimationController::CAnimationController()
{
  mIsPlaying = false;
  mTimer = 0;
  ActiveAnimationIndex = 0;
  mAnimSpeed = 1;
}

CAnimationController::~CAnimationController()
{

}

void CAnimationController::OnStart()
{
  RequireDependency<CSkinMeshRenderer>();
  Component::OnStart();

  // Default anim
  if(!mSingleAnim.Valid())
	mSingleAnim = AssetManager::getInstance()->GetAsset("MaleCharacterIdle.anim").GetAssetPtr<Animation>();
}

void CAnimationController::OnFree()
{
}

void CAnimationController::OnInit()
{
}

void CAnimationController::OnUpdate()
{

}

void CAnimationController::SetAnimation(const std::string& animName)
{
  mSingleAnim = AssetManager::getInstance()->GetAsset(animName).GetAssetPtr<Animation>();
}

void CAnimationController::AddAnimation(AssetHandle& anim)
{
  mAnimations.push_back(anim);
}

Animation* CAnimationController::GetActiveAnimation()
{
  // Fix this later to support multiple animations

  //if (mAnimations.size())
  //  return mAnimations[ActiveAnimationIndex].GetAssetPtr<Animation>();
  //else
  //  return 0;
  return mSingleAnim.GetAssetPtr<Animation>();
}

std::vector<VQS>& CAnimationController::GetFinalTransforms()
{
  return mFinalTransforms;
}

void CAnimationController::Bind()
{
	if (!gameObject->HasComponent<CSkinMeshRenderer>())
		return;

  SkinnedMesh* sMesh = static_cast<SkinnedMesh*>(gameObject->GetComponent<CSkinMeshRenderer>()->GetMesh());
  if(sMesh->GetMeshType() != MESH_SKINNED)
    return;

  if (sMesh)
  {
    static std::vector<VQS> transforms;
    transforms.resize(mFinalTransforms.size());
    for (unsigned i = 0; i < transforms.size(); ++i)
      transforms[i] = mFinalTransforms[i] * sMesh->GetSkeleton()->GetBones()[i].mBoneSpaceTransform;

    ConstantBufferManager::BindAnimBuffer(transforms);
  }  
}

meta_define(CAnimationController);
___COMP_REGISTER(CAnimationController);