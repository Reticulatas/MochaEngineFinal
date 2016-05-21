/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CMeshRenderer.h"
#include "CTransform.h"

void CMeshRenderer::OnFree(void)
{
  //mMat.Release();
  //if(GetState())
  // GetState()->RemoveMeshRenderer(this);
}

void CMeshRenderer::OnInit( void )
{
  //mMat = AssetManager::getInstance()->GetAsset<Material>("Diffuse.mat");

  /*
  TextureNode* diffuseTexture = new TextureNode();
  diffuseTexture->SetAttributeName("diffuseTexture");
  diffuseTexture->SetTexture("bg.png");

  ColorNode* tintColor = new ColorNode();
  tintColor->SetAttributeName("tintColor");
  tintColor->SetColor(COLOR::WHITE);

  UVTransformNode* uvTransform = new UVTransformNode();
  uvTransform->SetAttributeName("uvTransform");

  mMaterial.AddAttribute(uvTransform);
  mMaterial.AddAttribute(tintColor);
  mMaterial.AddAttribute(diffuseTexture);
  */

  //GetMaterial()->SetTexture("crate2.jpg");

  CRenderer::OnInit();
}

void CMeshRenderer::OnStart(void)
{
  CRenderer::OnStart();
  mBlendState = NO_BLEND_DEPTH_LESS_EQUAL;
}

void CMeshRenderer::OnUpdate(void)
{

}

CMeshRenderer::CMeshRenderer() : CRenderer()
{
  
}

CMeshRenderer::~CMeshRenderer()
{

}


meta_define(CMeshRenderer);
___COMP_REGISTER(CMeshRenderer);
/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change
