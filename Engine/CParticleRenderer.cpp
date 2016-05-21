#include "stdafx.h"

#include "CParticleRenderer.h"
#include "CParticleEmitter.h"
#include "AssetManager.h"
#include "CTransform.h"
#include"ColorNode.h"
#include "TextureNode.h"
#include "UVTransformNode.h"
#include "EditorSubsystem_Interface.h"

CParticleRenderer::CParticleRenderer()
{

}

void CParticleRenderer::OnFree( void )
{
}

void CParticleRenderer::OnInit( void )
{
  //mMesh = AssetManager::getInstance()->GetAsset("ParticleMesh");

    mMaterial.SetTexture("Metalball.png");
    mMaterial.SetShader("MochaParticleRenderTest.ms");
    
    mPS = mMaterial.GetShader()->GetPixelShader();
    mVS = mMaterial.GetShader()->GetVertexShader();
    mGS = mMaterial.GetShader()->GetGeometryShader();


  /*TextureNode* diffuseTexture = new TextureNode();
  diffuseTexture->SetAttributeName("diffuseTexture");
  diffuseTexture->SetTexture("bg.png");

  ColorNode* tintColor = new ColorNode();
  tintColor->SetAttributeName("tintColor");
  tintColor->SetColor(COLOR::WHITE);

  UVTransformNode* uvTransform = new UVTransformNode();
  uvTransform->SetAttributeName("uvTransform");

  mMaterial.AddAttribute(uvTransform);
  mMaterial.AddAttribute(tintColor);
  mMaterial.AddAttribute(diffuseTexture);*/

  //blendState = ALPHA_BLEND;
}

void CParticleRenderer::OnStart( void )
{
    CRenderer::OnStart();
    CRenderer::setEnabled(false);
    RequireDependency<CParticleEmitter>();
}

void CParticleRenderer::OnUpdate( void )
{
    
}

void CParticleRenderer::OnDraw(void)
{

}


Mesh* CParticleRenderer::GetMesh()
{
  return mMesh.GetAssetPtr<Mesh>();
}

Material* CParticleRenderer::GetMaterial()
{
  return &mMaterial;
}


CParticleRenderer::~CParticleRenderer()
{

}

___COMP_REGISTER(CParticleRenderer);
meta_define(CParticleRenderer);
/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change