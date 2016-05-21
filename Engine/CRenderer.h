#pragma once

#include "AssetManager.h"
#include "Material.h"
#include "CCamera.h"
#include "AdjancencyInfo.h"
class Shader;
class CCamera;
class Material;
class Mesh;

class CRenderer : public Component
{
public:

  enum BlendState
  {
    ALPHA_BLEND,
    ADDITIVE_BLEND,
    DEFERRED_BLENDPASS_1,
    DEFERRED_BLENDPASS_2,
    COLOR_BLEND,
    NO_BLEND_DEPTH_LESS,
    NO_BLEND_DEPTH_LESS_EQUAL
  };

  CRenderer();
  virtual ~CRenderer();

  void OnFree(void);
  void OnInit(void);
  void OnStart(void);
  void OnUpdate(void);
  void OnDraw(void);

  virtual void SetMesh(AssetHandle& meshHandle);
  virtual void SetMaterial(Material& matHandle);
  virtual Mesh* GetMesh() const;
  virtual Material* GetMaterial();
  virtual Material* GetCopyMaterial();
  virtual void BindBuffers(const CCamera* cam, Shader* overrideShader = 0);
  virtual void BindEarlyZBuffers(const CCamera* cam, Shader* overrideShader = 0);
  virtual void GetFrustumCullData(Vector3& origin, Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;
  virtual Vector3 GetMeshDimension() const;
  AABB GetBoundingBox() const;
  virtual bool IsReceiveShadows() const;

  void DrawSelectedMesh();

  //Sort functor
  struct sortBackToFront
  {
    sortBackToFront(CCamera* c) : cam(c) {}
    bool operator()(CRenderer *lhs, CRenderer *rhs) const;
    CCamera* cam;
  };

  struct sortFrontToBack
  {
    bool operator()(CRenderer *lhs, CRenderer *rhs) const;
    CCamera* cam;
  };

  // Public Member Variables - Expose to the editor
  AssetHandle mMesh;
  Material mMaterial;
  BlendState mBlendState;
  float mBumpFactor;
  float mMetallicFactor;
  float mRoughnessFactor;
  DEPRECATED int blendState;
  Color mEmissiveColor;
  bool mPerspective;
  bool mReceiveShadows;

  virtual void CopyFrom(const ::CRenderer* c)
  {
    mMesh = c->mMesh;
    mMaterial = c->mMaterial;
    mBlendState = c->mBlendState;
    mBumpFactor = c->mBumpFactor;
	  mRoughnessFactor = c->mRoughnessFactor;
	  mMetallicFactor = c->mMetallicFactor;
    mPrevWorldViewProj = c->mPrevWorldViewProj;
	  mEmissiveColor = c->mEmissiveColor;
    mPerspective = c->mPerspective;
    mReceiveShadows = c->mReceiveShadows;
    Component::CopyFrom(c);
  }
 // COMPONENTCLONE(CRenderer);


private:
  std::map<unsigned, Matrix4> mPrevWorldViewProj;
public:
  ___SERIALIZE_SAVE_BEGIN___
	  ___DEFBASE(Component)
	  ___DEFSER(mMesh,1)
	  ___DEFSER(mMaterial, 1)
	  ___DEFSER(blendState, 1)
    ___DEFSER(mBumpFactor, 2)
	___DEFSER(mMetallicFactor, 5)
	___DEFSER(mRoughnessFactor, 5)
    ___DEFSER(mBlendState, 3)
	___DEFSER(mEmissiveColor, 4)
  ___DEFSER(mPerspective, 6)
  ___DEFSER(mReceiveShadows, 7)
	  ___SERIALIZE_END___

	  ___SERIALIZE_LOAD_BEGIN___
	  ___DEFBASE(Component)
	  ___DEFSER(mMesh, 1)
	  SetMesh(mMesh);
    ___DEFSER(mMaterial, 1)
    ___DEFSER(blendState, 1)
    ___DEFSER(mBumpFactor, 2)
	___DEFSER(mMetallicFactor, 5)
	___DEFSER(mRoughnessFactor, 5)
    ___DEFSER(mBlendState, 3)
	___DEFSER(mEmissiveColor, 4)
  ___DEFSER(mPerspective, 6)
  ___DEFSER(mReceiveShadows, 7)
  ___SERIALIZE_END___

  ___SERIALIZE_SAVE_LOAD___

public:
    metabasedef(CRenderer)
      m_addbase(Component)
    m_add(int, blendState)
    endmetabasedef;
};
___SERIALIZE_CLASS(CRenderer, 7);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(CRenderer);