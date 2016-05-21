/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Eduardo Chaves
-* See Footer for Revisions                                      */


#pragma once

#include "CCamera.h"

class Shader;
class CCamera;
class Material;
class Mesh;
#ifdef MOCHAEDITOR
#include "Component.h"
#endif


class CReflectionProbe : public CCamera
{
	unsigned mCubemapWidth;
	RenderTarget* mReflectionRT;
	bool mRefreshProbe; // Tells the renderer "Draw me like one of your French girls"

public:

	Vector3 mProbeBoxDim;
	bool mStatic;

	CReflectionProbe(void);
	virtual ~CReflectionProbe();

	void OnFree(void);
	void OnInit(void);
	void OnStart(void);
	void OnUpdate(void);
	void OnEditorUpdate(void);
	void OnDraw(void);
	virtual void Resize();

	virtual RenderTarget* GetRenderTarget(void) const;
	virtual void SetupCameraDirection(unsigned cubemapDirectionIndex = 0);

	bool ShouldRefresh(void);
  virtual void OnRefreshed(void);

	const Vector3& GetBoxDimentions(void);

	virtual void CopyFrom(const ::CReflectionProbe* c)
	{
		CCamera::CopyFrom(c);
		mCubemapWidth = c->mCubemapWidth;
		mReflectionRT = c->mReflectionRT;
		mRefreshProbe = c->mRefreshProbe;
		mStatic = c->mStatic;
		mProbeBoxDim = c->mProbeBoxDim;
	}
	COMPONENTCLONE(CReflectionProbe);

	___SERIALIZE_BEGIN___
	___DEFBASE(CCamera)
	___DEFSER(mCubemapWidth, 1)
	___DEFSER(mReflectionRT, 1)
	___DEFSER(mRefreshProbe, 1)
	___DEFSER(mStatic, 1)
	___DEFSER(mProbeBoxDim, 2)
	___SERIALIZE_END___

	metadef(CReflectionProbe)
  m_addbase(Component)
	endmetadef;
};

___SERIALIZE_CLASS(CReflectionProbe, 2);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change