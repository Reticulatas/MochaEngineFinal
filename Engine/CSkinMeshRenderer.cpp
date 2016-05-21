#include "stdafx.h"
#include "CSkinMeshRenderer.h"
#include "CTransform.h"
#include "TextureNode.h"
#include "CAnimationController.h"

CSkinMeshRenderer::CSkinMeshRenderer() : CRenderer()
{  
}

CSkinMeshRenderer::~CSkinMeshRenderer()
{
}

void CSkinMeshRenderer::OnFree()
{
}

void CSkinMeshRenderer::OnInit(void)
{
  // Default material shader to MochaAnimation Shader
  CRenderer::OnInit();
  mMaterial.SetShader("MochaAnimation.ms");
}

void CSkinMeshRenderer::OnStart(void)
{
  if (!mMesh.Valid())
    mMesh = AssetManager::getInstance()->GetAsset("Man.bean").GetAssetPtr<Mesh>();

  if (!mMaterial.GetAttribute<TextureNode>()->GetTexture().Valid())
    mMaterial.SetTexture("default.png");

  Component::OnStart();
  mBlendState = NO_BLEND_DEPTH_LESS_EQUAL;
}

void CSkinMeshRenderer::SetMesh(AssetHandle& meshHandle)
{
  mMesh = meshHandle.GetAssetPtr<SkinnedMesh>();
}

void CSkinMeshRenderer::OnDraw()
{
  if (gameObject->mIsSelected)
  {
    if (GetMesh() && GetMesh()->GetMeshType() == MESH_SKINNED)
    {
      SkinnedMesh* sMesh = static_cast<SkinnedMesh*>(GetMesh());
      if (sMesh && gameObject->HasComponent<CAnimationController>())
        sMesh->GetSkeleton()->DrawBones(gameObject->GetComponent<CAnimationController>()->GetFinalTransforms(), gameObject->transform->GetWorldMatrix());
    }
  }
}

void CSkinMeshRenderer::BindBuffers(const CCamera* cam, Shader* overrideShader)
{
  CRenderer::BindBuffers(cam, overrideShader);
  InputLayouts::SetInputLayout(InputLayouts::InputLayout_SkinnedVert);

  CAnimationController* animCtrl = gameObject->GetComponent<CAnimationController>();
  if (animCtrl)
    animCtrl->Bind();
}


void CSkinMeshRenderer::BindEarlyZBuffers(const CCamera* cam, Shader* overrideShader /*= 0*/)
{
  CRenderer::BindEarlyZBuffers(cam, overrideShader);
  InputLayouts::SetInputLayout(InputLayouts::InputLayout_SkinnedVert);

  CAnimationController* animCtrl = gameObject->GetComponent<CAnimationController>();
  if (animCtrl)
    animCtrl->Bind();
}

meta_define(CSkinMeshRenderer);
___COMP_REGISTER(CSkinMeshRenderer);