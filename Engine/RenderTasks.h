#pragma once
#include "Task.h"
#include <future>

class FrustumCullingTask : public Task
{
public:
  FrustumCullingTask();
  virtual ~FrustumCullingTask();

  virtual bool Tick() override;

  void FrustrumCull(CCamera* cam, BaseState* state);
  void FrustumCullToGo(CCamera* cam);

  static FrustumCullingTask* getInstance();

  metadef(FrustumCullingTask)
  endmetadef;
};

class RenderOculusTask : public Task{
public:

  virtual bool Tick() override;

  void RenderOculus();

  CCamera* oculusCam;
  RenderTarget* oculusRT;
  std::future<void> future;
  std::promise<void> promise;

  metadef(RenderOculusTask)
    endmetadef;
};