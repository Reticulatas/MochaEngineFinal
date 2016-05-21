/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CDiffuseRenderer.h"

void CDiffuseRenderer::OnInit(void)
{
  CRenderer::OnInit();
  mDrawOnTop = false;
  mBlendState = ALPHA_BLEND;
}

void CDiffuseRenderer::OnStart(void)
{
  mMaterial.SetShader("MochaDiffuseRender.ms");
  CRenderer::OnStart();
}

CDiffuseRenderer::CDiffuseRenderer() : CRenderer()
{  
}

CDiffuseRenderer::~CDiffuseRenderer()
{
}

void CDiffuseRenderer::BindBuffers(const CCamera* cam, Shader* overrideShader /*= 0*/)
{
  CRenderer::BindBuffers(cam, overrideShader);

  if (mDrawOnTop)
    Renderer::getInstance()->EnableDepthWriting(false);
  else
    Renderer::getInstance()->EnableDepthWriting(true);
}

meta_define(CDiffuseRenderer);
___COMP_REGISTER(CDiffuseRenderer);
/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change
