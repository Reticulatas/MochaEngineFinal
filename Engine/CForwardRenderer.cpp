/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CForwardRenderer.h"
#include "CTransform.h"

void CForwardRenderer::OnFree(void)
{

}

void CForwardRenderer::OnInit(void)
{
	CRenderer::OnInit();
	mBlendState = ALPHA_BLEND;
	mDrawOnTop = false;
}

void CForwardRenderer::OnStart(void)
{
  mMaterial.SetShader("MochaForwardRender.ms");
  CRenderer::OnStart();
}

void CForwardRenderer::OnUpdate(void)
{

}

CForwardRenderer::CForwardRenderer() : CRenderer()
{
  
}

CForwardRenderer::~CForwardRenderer()
{

}

void CForwardRenderer::BindBuffers(const CCamera* cam, Shader* overrideShader /*= 0*/)
{
  CRenderer::BindBuffers(cam, overrideShader);

  if (mDrawOnTop)
    Renderer::getInstance()->EnableDepthWriting(false);
  else
    Renderer::getInstance()->EnableDepthWriting(true);
}


meta_define(CForwardRenderer);
___COMP_REGISTER(CForwardRenderer);
/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change
