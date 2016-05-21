#pragma once

#include "CRenderer.h"
#include "SkinnedMesh.h"

class CSkinMeshRenderer : public CRenderer
{
public:
  CSkinMeshRenderer();
  virtual ~CSkinMeshRenderer();

  void OnInit(void);
  void OnStart(void);
  void OnDraw();
  void OnFree();

  virtual void BindBuffers(const CCamera* cam, Shader* overrideShader = 0);
  virtual void BindEarlyZBuffers(const CCamera* cam, Shader* overrideShader = 0);
  virtual void SetMesh(AssetHandle& meshHandle);

  virtual void CopyFrom(const ::CSkinMeshRenderer* c)
  {
    CRenderer::CopyFrom(c);
  }
  COMPONENTCLONE(CSkinMeshRenderer);

private:

  ___SERIALIZE_BEGIN___
    ___DEFBASE(CRenderer)
    ___SERIALIZE_END___

public:
  metadef(CSkinMeshRenderer)
    m_addbase(CRenderer)
    endmetadef;
};
___SERIALIZE_CLASS(CSkinMeshRenderer, 1);
