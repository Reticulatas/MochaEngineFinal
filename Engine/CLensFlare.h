#pragma once

#include "CPostProcess.h"
#include "RenderTarget.h"

class CLensFlare : public CPostProcess
{
public:

	float mDispersionFactor;
	float mGhostCount;
	float mFlareStrenght;
	AssetHandle mDirtyLensMask;

/////////////////////////////////////////
	CLensFlare();
	virtual ~CLensFlare();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam);

  virtual void CopyFrom(const ::CLensFlare* c)
  {
	  mDispersionFactor = c->mDispersionFactor;
	  mGhostCount = c->mGhostCount;
	  mDirtyLensMask = c->mDirtyLensMask;
	  mFlareStrenght = c->mFlareStrenght;
	  CPostProcess::CopyFrom(c);
  }

  COMPONENTCLONE(CLensFlare);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mDispersionFactor, 1)
	  ___DEFSER(mGhostCount, 1)
	  ___DEFSER(mDirtyLensMask, 2)
	  ___DEFSER(mFlareStrenght, 2)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mDispersionFactor, 1)
	  ___DEFSER(mGhostCount, 1)
	  ___DEFSER(mDirtyLensMask, 2)
	  ___DEFSER(mFlareStrenght, 2)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
	metadef(CLensFlare)
		m_addbase(CPostProcess)
    endmetadef;
};
___SERIALIZE_CLASS(CLensFlare, 2);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CLensFlare);
