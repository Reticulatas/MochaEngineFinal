/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#pragma once

#include "CRenderer.h"

class Shader;
class CCamera;
class Material;
class Mesh;

class CDiffuseRenderer : public CRenderer
{
public:
  CDiffuseRenderer(void);
  virtual ~CDiffuseRenderer();

	void OnInit(void);
	void OnStart(void);
  void BindBuffers(const CCamera* cam, Shader* overrideShader = 0);

  virtual void CopyFrom(const ::CDiffuseRenderer* c)
	{
		CRenderer::CopyFrom(c);
    mDrawOnTop = c->mDrawOnTop;
	}
  COMPONENTCLONE(CDiffuseRenderer);

  bool mDrawOnTop;

	___SERIALIZE_BEGIN___
	___DEFBASE(CRenderer)
  ___DEFSER(mDrawOnTop, 2)
	___SERIALIZE_END___

  metadef(CDiffuseRenderer)
  m_addbase(CRenderer)
	endmetadef;
};

___SERIALIZE_CLASS(CDiffuseRenderer, 2);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change