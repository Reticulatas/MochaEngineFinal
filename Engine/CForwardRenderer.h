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
#ifdef MOCHAEDITOR
#include "Component.h"
#endif


class CForwardRenderer : public CRenderer
{
public:
  CForwardRenderer(void);
  virtual ~CForwardRenderer();

	void OnFree(void);
	void OnInit(void);
	void OnStart(void);
	void OnUpdate(void);

  virtual void BindBuffers(const CCamera* cam, Shader* overrideShader = 0);
  virtual void CopyFrom(const ::CForwardRenderer* c)
	{
		CRenderer::CopyFrom(c);
    mDrawOnTop = c->mDrawOnTop;
	}
  COMPONENTCLONE(CForwardRenderer);

public:

  bool mDrawOnTop;

	___SERIALIZE_BEGIN___
		___DEFBASE(CRenderer)
    ___DEFSER(mDrawOnTop, 1)
		___SERIALIZE_END___

    metadef(CForwardRenderer)
    m_addbase(CRenderer)
		endmetadef;
};

___SERIALIZE_CLASS(CForwardRenderer, 1);

/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change