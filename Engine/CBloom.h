#pragma once

#include "CPostProcess.h"

class CBloom : public CPostProcess
{
public:

	float mBloomFactor;

/////////////////////////////////////////
  CBloom();
  virtual ~CBloom();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam);

  virtual void CopyFrom(const ::CBloom* c)
  {
	  mBloomFactor = c->mBloomFactor;
	  CPostProcess::CopyFrom(c);
  }

 COMPONENTCLONE(CBloom);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mBloomFactor, 2)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mBloomFactor, 2)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
	metadef(CBloom)
		m_addbase(CPostProcess)
    endmetadef;
};
___SERIALIZE_CLASS(CBloom, 2);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CBloom);