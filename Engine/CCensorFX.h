#pragma once

#include "CPostProcess.h"

class CCensorFX : public CPostProcess
{
public:

/////////////////////////////////////////
	CCensorFX();
	virtual ~CCensorFX();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam);

  virtual void CopyFrom(const ::CCensorFX* c)
  {
	  CPostProcess::CopyFrom(c);
  }

  COMPONENTCLONE(CCensorFX);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(CPostProcess)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
	metadef(CCensorFX)
		m_addbase(CPostProcess)
    endmetadef;
};
___SERIALIZE_CLASS(CCensorFX, 1);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CCensorFX);