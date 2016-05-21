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


class CMeshRenderer : public CRenderer
{
public:
	CMeshRenderer(void);
	virtual ~CMeshRenderer();

	void OnFree(void);
	void OnInit(void);
	void OnStart(void);
	void OnUpdate(void);

	virtual void CopyFrom(const ::CMeshRenderer* c)
	{
		CRenderer::CopyFrom(c);
	}
	COMPONENTCLONE(CMeshRenderer);

	___SERIALIZE_BEGIN___
	___DEFBASE(CRenderer)
	___SERIALIZE_END___

	metadef(CMeshRenderer)
  m_addbase(CRenderer)
	endmetadef;
};

___SERIALIZE_CLASS(CMeshRenderer, 2);

/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change