/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#pragma once
#include "CRenderer.h"

#define MAX_FORWARDLIGHTS 50

struct LightProperties
{
  Vector4 mDiffuse;

  Vector3 mPosition;
  float mRange;

  Vector3 mDirection;
  float mAtt;  

  float mInnerAngle;      // Inner spotlight angle (DEGREES)
  float mOuterAngle;      // Outer spotlight angle (DEGREES)
  float mSpot;

  // LIGHTTYPE
  // 0 for DIRECTIONAL
  // 1 for POINT
  // 2 for SPOTLIGHT
  int mLightType;

  ___SERIALIZE_BEGIN___
    ___DEFSER(mDiffuse, 1)
    ___DEFSER(mRange, 1)
    ___DEFSER(mDirection, 1)
    ___DEFSER(mSpot, 1)
    ___DEFSER(mAtt, 1)
    ___DEFSER(mLightType, 1)
    ___SERIALIZE_END___
};
___SERIALIZE_CLASS(LightProperties, 1);

class CLight : public CRenderer
{
public:

  enum LIGHTTYPE
  {
    DIRECTIONAL,
    POINT,
    SPOTLIGHT
  };

  CLight();
	virtual ~CLight();

  void OnStart();
  void OnInit();
  void OnUpdate();
  void OnFree();
  void OnDraw();
  void OnEditorUpdate() override;

  virtual void SetMesh(AssetHandle& meshHandle);
  virtual void BindBuffers(const CCamera* cam, Shader* overrideShader = 0);
  Matrix4 GetRangedWorldMatrix();

  // Gettors & Settors 
  Color GetDiffuse() const { return mDiffuse; }
  void SetDiffuse(Color val) { mDiffuse = val; }
  float GetRange() const { return mRange; }
  void SetRange(float val) { mRange = val; }
  float GetSpot() const { return mSpot; }
  void SetSpot(float val) { mSpot = val; }
  float GetAtt() const { return mAtt; }
  void SetAtt(float val) { mAtt = val; }
  int GetLightType() const { return mLightType; }
  void SetLightType(LIGHTTYPE val);
  float GetIntensity() const { return mIntensity; }

  LightProperties GetLightProperties() const;

  // Sort functor
  struct sortLights {
    bool operator()( CLight *lhs, CLight *rhs ) const
    { return lhs->mLightType < rhs->mLightType; }
  };

  ___BEGIN_REGISTERED___
  Color mDiffuse;
  float mRange;         // Packed into 4D vector: (Position, Range)
  float mAtt;
  float mInnerAngle;      // Inner spotlight angle (DEGREES)
  float mOuterAngle;      // Outer spotlight angle (DEGREES)
  float mSpot;
  float mIntensity;
  LIGHTTYPE mLightType;
  Vector3 mLightDir;
  ___END_REGISTERED___

  virtual void CopyFrom(const ::CLight* c)
  {
    mDiffuse = c->mDiffuse;
    mRange = c->mRange;
    mSpot = c->mSpot;
    mAtt = c->mAtt;
    mLightType = c->mLightType;
    mOuterAngle = c->mOuterAngle;
    mInnerAngle = c->mInnerAngle;
    mIntensity = c->mIntensity;
    mLightProperties = c->mLightProperties;
    mLightDir = c->mLightDir;
    Component::CopyFrom(c);
  }
  COMPONENTCLONE(CLight);

private:
  ___SERIALIZE_BEGIN___
    ___DEFBASE(Component)
    ___DEFSER(mDiffuse, 1)
    ___DEFSER(mRange, 1)
    ___DEFSER(mSpot, 1)
    ___DEFSER(mAtt, 1)
    ___DEFSER(mLightType, 1)
    ___DEFSER(mLightProperties, 1)
    ___DEFSER(mInnerAngle, 2)
    ___DEFSER(mOuterAngle, 2)
    ___DEFSER(mLightDir, 2)
    ___DEFSER(mIntensity, 3)
    ___SERIALIZE_END___
  LightProperties mLightProperties;
  LIGHTTYPE mPrevLightType;

public:
	metadef(CLight)
		m_add(float, mRange)
		m_add(float, mSpot)
    m_add(LIGHTTYPE, mLightType)
		endmetadef;

	virtual void OnEditorStart(void);

	virtual void OnEditorFree(void);

};
___SERIALIZE_CLASS(CLight, 3);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change