#pragma once

#include "CPostProcess.h"

class CMotionBlur : public CPostProcess
{
public:

	int mBlurFactor;

/////////////////////////////////////////
	CMotionBlur();
	virtual ~CMotionBlur();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam);

  virtual void CopyFrom(const ::CMotionBlur* c)
  {
	  mBlurFactor = c->mBlurFactor;
	  CPostProcess::CopyFrom(c);
  }

  COMPONENTCLONE(CMotionBlur);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mBlurFactor, 1)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mBlurFactor, 1)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
	metadef(CMotionBlur)
		m_addbase(CPostProcess)
    endmetadef;
};
___SERIALIZE_CLASS(CMotionBlur, 1);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CMotionBlur);