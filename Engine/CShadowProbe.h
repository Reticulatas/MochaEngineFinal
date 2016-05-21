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


class CShadowProbe : public CCamera
{
	unsigned mCubemapWidth;
  RenderTarget* mShadowMap;
	bool mRefreshProbe; // Tells the renderer "Draw me like one of your French girls"

public:

	bool mStatic;
  float mStrength;
  float mBias;
  bool mUseSoftShadows;

  CShadowProbe(void);
  virtual ~CShadowProbe();

	void OnFree(void);
	void OnInit(void);
	void OnStart(void);
	void OnUpdate(void);
	void OnEditorUpdate(void);
	void OnDraw(void);
  virtual void OnEditorStart(void) override;
  virtual void OnEditorFree(void) override;

	virtual RenderTarget* GetRenderTarget(void) const;
	virtual void SetupCameraDirection(unsigned cubemapDirectionIndex = 0);

  bool IsValid();
	bool ShouldRefresh(void);
  virtual void OnRefreshed(void);

  virtual void CopyFrom(const ::CShadowProbe* c)
	{
		CCamera::CopyFrom(c);
		mCubemapWidth = c->mCubemapWidth;
		mShadowMap = c->mShadowMap;
		mRefreshProbe = c->mRefreshProbe;
		mStatic = c->mStatic;
    mStrength = c->mStrength;
    mBias = c->mBias;
    mUseSoftShadows = c->mUseSoftShadows;
	}
  COMPONENTCLONE(CShadowProbe);

	___SERIALIZE_BEGIN___
	___DEFBASE(CCamera)
	___DEFSER(mCubemapWidth, 1)
	___DEFSER(mRefreshProbe, 1)
	___DEFSER(mStatic, 1)
  ___DEFSER(mStrength, 2)
  ___DEFSER(mBias, 2)
  ___DEFSER(mUseSoftShadows, 3)
	___SERIALIZE_END___

  metadef(CShadowProbe)
  m_addbase(Component)
	endmetadef;
};

___SERIALIZE_CLASS(CShadowProbe, 3);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change