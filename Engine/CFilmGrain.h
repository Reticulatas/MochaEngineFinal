#pragma once

#include "CPostProcess.h"

class CFilmGrain : public CPostProcess
{
public:

	float mBarHeight;
	float mBarSpeed;
	float mBarOverflow;
	float mNoiseDensity;
	float mPixelDensity;
/////////////////////////////////////
/////////////////////////////////////

  CFilmGrain();
  virtual ~CFilmGrain();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam);

  virtual void CopyFrom(const ::CFilmGrain* c)
  {
	  mBarHeight = c->mBarHeight;
	  mBarSpeed = c->mBarSpeed;
	  mBarOverflow = c->mBarOverflow;
	  mNoiseDensity = c->mNoiseDensity;
	  mPixelDensity = c->mPixelDensity;
	  CPostProcess::CopyFrom(c);
  }

  COMPONENTCLONE(CFilmGrain);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mBarHeight, 1)
	  ___DEFSER(mBarSpeed, 1)
	  ___DEFSER(mBarOverflow, 1)
	  ___DEFSER(mNoiseDensity, 1)
	  ___DEFSER(mPixelDensity, 1)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(CPostProcess)
	  ___DEFSER(mBarHeight, 1)
	  ___DEFSER(mBarSpeed, 1)
	  ___DEFSER(mBarOverflow, 1)
	  ___DEFSER(mNoiseDensity, 1)
	  ___DEFSER(mPixelDensity, 1)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
	metadef(CFilmGrain)
		m_addbase(CPostProcess)
    endmetadef;
};
___SERIALIZE_CLASS(CFilmGrain, 1);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CFilmGrain);