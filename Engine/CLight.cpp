/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CLight.h"
#include "CTransform.h"
#include "ConstantBuffer.h"
#include "EditorSubsystem.h"
#include "CShadowProbe.h" 

CLight::CLight() : CRenderer()
{}

void CLight::OnInit()
{
  mDiffuse = COLOR::WHITE;
  mRange = 10;
  mSpot = 1.f;
  mAtt = 1.f;
  mInnerAngle = 30;
  mOuterAngle = 45;
  mLightDir = Vector3(0, -1, 0);
  mIntensity = 1;

  // Default to point lights
  mLightType = POINT;
  mPrevLightType = (LIGHTTYPE) - 1;  
  SetLightType(POINT);

  
  mBlendState = DEFERRED_BLENDPASS_1;
}

void CLight::OnStart()
{
  Component::OnStart();
}

void CLight::OnUpdate()
{
	SetLightType(mLightType);
}

void CLight::OnFree()
{
  //Renderer::getInstance()->RemoveLightComponent(this);
}

void CLight::SetLightType(LIGHTTYPE val)
{
  if (val != mPrevLightType)
  {
    mLightType = mPrevLightType = val;

    switch (mLightType)
    {
    case DIRECTIONAL:
      mMesh = AssetManager::getInstance()->GetAsset("Quad.bean").GetAssetPtr<Mesh>();
      break;

    case POINT:
      mMesh = AssetManager::getInstance()->GetAsset("Sphere.bean").GetAssetPtr<Mesh>();
      break;

    case SPOTLIGHT:
      mMesh = AssetManager::getInstance()->GetAsset("Sphere.bean").GetAssetPtr<Mesh>();
      break;
    }
  }  
}

LightProperties CLight::GetLightProperties() const
{
  LightProperties mLightProp;

  mLightProp.mDiffuse    = mDiffuse.ToVector4();
  mLightProp.mPosition   = gameObject->transform->position();
  mLightProp.mRange      = mRange;  
  mLightProp.mSpot       = mSpot;
  mLightProp.mAtt        = mAtt;
  mLightProp.mInnerAngle = Math::Cos(DegreesToRadian(mInnerAngle));
  mLightProp.mOuterAngle = Math::Cos(DegreesToRadian(mOuterAngle));
  mLightProp.mLightType  = mLightType;
  // Directional has to be based on light direction
  if (mLightType == DIRECTIONAL)
    mLightProp.mDirection = mLightDir;
  // Spotlight has to be based on objects orientation - (0,-1,0) is the default orientation of the light
  else
    mLightProp.mDirection = TransformNormal(gameObject->transform->GetGlobalRotationMatrix(), Vector3(0, -1, 0));

  return mLightProp;
}

CLight::~CLight()
{

}

void CLight::SetMesh(AssetHandle& meshHandle)
{
  mMesh = meshHandle.GetAssetPtr<Mesh>();
}

void CLight::BindBuffers(const CCamera* cam, Shader* overrideShader /*= 0*/)
{
  InputLayouts::SetInputLayout(InputLayouts::InputLayout_Vert);
  //Shader* mShader = (overrideShader) ? overrideShader : mMaterial.GetShader();
  
  switch (mLightType)
  {
  case DIRECTIONAL:
    ConstantBufferManager::BindCameraBuffer(cam->PostProcessMatrix());
    if (!Math::IsZero(cam->Proj().Determinant()))
      ConstantBufferManager::BindMeshBuffer(cam->Proj().Inverted());
    else
      ConstantBufferManager::BindMeshBuffer(Matrix4());
    break;

  case POINT:
  case SPOTLIGHT:
    Matrix4 World = GetRangedWorldMatrix();
    // Correct - it is P V W
    Matrix4 WorldViewProj = cam->ProjView() * World;
    ConstantBufferManager::BindCameraBuffer(WorldViewProj);
    ConstantBufferManager::BindMeshBuffer(cam->View() * World);
    break;
  }
}

Math::Matrix4 CLight::GetRangedWorldMatrix()
{
  Matrix4 m;

  switch (mLightType)
  {
  case DIRECTIONAL:
    m = gameObject->transform->GetWorldMatrix();

  case POINT:
  case SPOTLIGHT:
    Matrix4 scale;
    scale.Scale(Vector3(mRange));
    m = gameObject->transform->GetGlobalTranslationMatrix() * gameObject->transform->GetGlobalRotationMatrix() * scale;
    break;
  }

  return m;
}

void CLight::OnDraw()
{
  if (gameObject->mIsSelected)
  {
    std::vector<Vector3> verts = mMesh.GetAssetPtr<Mesh>()->GetUniqueVertices();
    std::vector<int> indices = mMesh.GetAssetPtr<Mesh>()->GetUnqiueIndices();

    Matrix4 world = GetRangedWorldMatrix();

    for (unsigned i = 0; i < indices.size(); i += 3)
    {
      unsigned index = indices[i];
      unsigned index1 = indices[i + 1];
      unsigned index2 = indices[i + 2];

      Vector3 pos0 = TransformPoint(world, verts[index]);
      Vector3 pos1 = TransformPoint(world, verts[index1]);
      Vector3 pos2 = TransformPoint(world, verts[index2]);

      Renderer::getInstance()->DrawLinePerma(pos0, pos1, COLOR::LIME_GREEN);

      if (mLightType == DIRECTIONAL)
      {
        Renderer::getInstance()->DrawLinePerma(pos0, pos2, COLOR::LIME_GREEN);
        Renderer::getInstance()->DrawLinePerma(pos1, pos2, COLOR::LIME_GREEN);
      }
    }
  }
}

void CLight::OnEditorUpdate()
{
	OnUpdate();
}

void CLight::OnEditorStart(void)
{
	EditorSubsystem::getInstance()->widgetManager.RegisterWidget(gameObject, EditorSubsystemTools::WidgetManager::LIGHT);
}

void CLight::OnEditorFree(void)
{
	EditorSubsystem::getInstance()->widgetManager.DeRegisterWidget(gameObject);
}

___COMP_REGISTER(CLight);
meta_define(CLight);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change