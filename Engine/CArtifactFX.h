#pragma once

#include "CPostProcess.h"

class CArtifactFX : public CPostProcess
{
public:

	float mBlockThreshold;
  float mLineThreshold;

/////////////////////////////////////////
  CArtifactFX();
  virtual ~CArtifactFX();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam);

  virtual void CopyFrom(const ::CArtifactFX* c)
  {
    mBlockThreshold = c->mBlockThreshold;
    mLineThreshold = c->mLineThreshold;
	  CPostProcess::CopyFrom(c);
  }

  COMPONENTCLONE(CArtifactFX);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(CPostProcess)
    ___DEFSER(mBlockThreshold, 1)
    ___DEFSER(mLineThreshold, 1)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(CPostProcess)
    ___DEFSER(mBlockThreshold, 1)
    ___DEFSER(mLineThreshold, 1)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
  metadef(CArtifactFX)
		m_addbase(CPostProcess)
    endmetadef;
};
___SERIALIZE_CLASS(CArtifactFX, 1);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CArtifactFX);