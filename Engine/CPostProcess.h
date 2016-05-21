#pragma once

#include "AssetManager.h"
#include "CCamera.h"
#include "AdjancencyInfo.h"
class Shader;
class CCamera;

class CPostProcess : public Component
{
public:

  CPostProcess();
  virtual ~CPostProcess();

  virtual void OnFree(void);
  virtual void OnInit(void);
  virtual void OnStart(void);
  virtual void OnUpdate(void);

  // Execute the 
  virtual void DoPostProcess(const CCamera* cam) = 0;

  virtual void CopyFrom(const ::CPostProcess* c)
  {
    Component::CopyFrom(c);
  }
  //COMPONENTCLONE(CPostProcess);


public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(Component)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(Component)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
	metabasedef(CPostProcess)
    endmetabasedef;
};
___SERIALIZE_CLASS(CPostProcess, 1); 
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CPostProcess);
