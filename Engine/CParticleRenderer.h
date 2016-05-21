#pragma once
#include "Component.h"

class Shader;
class CCamera;
class Material;
class Mesh;

struct ParticleRenderConstants
{
    Matrix gView;
    Matrix gProjection;
    //Matrix gWorldViewProj;
    Vector3 camPos;
    float   quadLength;
};

class CParticleRenderer : public CRenderer
{
public:

	~CParticleRenderer();
  CParticleRenderer();

  void OnFree(void);
  void OnInit(void);
  void OnStart(void);
  void OnUpdate(void);
  void OnDraw(void);

  //virtual void SetMesh(AssetHandle& meshHandle);
  virtual Mesh* GetMesh();
  virtual Material* GetMaterial();
  //virtual Material* GetCopyMaterial();
  //virtual void BindBuffers(const CCamera* cam, Shader* overrideShader = 0);
  ParticleRenderConstants   mRenderConstants;

  SubShader<ID3D11PixelShader>    * mPS = nullptr;
  SubShader<ID3D11VertexShader>   * mVS = nullptr;
  SubShader<ID3D11GeometryShader> * mGS = nullptr;

  virtual void CopyFrom(const ::CParticleRenderer* c)
  {
    mMesh = c->mMesh;
    mMaterial = c->mMaterial;
    blendState = c->blendState;
    Component::CopyFrom(c);
  }
  COMPONENTCLONE(CParticleRenderer);

  ___SERIALIZE_BEGIN___
	  ___DEFBASE(CRenderer)
    ___SERIALIZE_END___

public:
	metadef(CParticleRenderer)
	    endmetadef;
};

___SERIALIZE_CLASS(CParticleRenderer, 1);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change