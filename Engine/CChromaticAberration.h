#pragma once

#include "CPostProcess.h"

class CChromaticAberration : public CPostProcess
{
public:

/////////////////////////////////////
/////////////////////////////////////

	CChromaticAberration();
	virtual ~CChromaticAberration();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam);

  virtual void CopyFrom(const ::CChromaticAberration* c)
  {
	  CPostProcess::CopyFrom(c);
  }

  COMPONENTCLONE(CChromaticAberration);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(CPostProcess)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
	metadef(CChromaticAberration)
		m_addbase(CPostProcess)
    endmetadef;
};
___SERIALIZE_CLASS(CChromaticAberration, 1);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CChromaticAberration);