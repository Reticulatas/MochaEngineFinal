%{
#include "CRenderer.h"
#include "AABB.h"
%}

struct AABB
{
	AABB();
	~AABB();
public:
	Math::Vector3 minPoint, maxPoint;
};

class CRenderer : public Component
{
	CRenderer();
	virtual ~CRenderer();

public:

  enum BlendState
  {
    ALPHA_BLEND,
    ADDITIVE_BLEND,
    DEFERRED_BLENDPASS_1,
    DEFERRED_BLENDPASS_2,
    COLOR_BLEND,
    NO_BLEND
  };

	virtual void SetMesh(AssetHandle& meshHandle) = 0;
	virtual void SetMaterial(Material& matHandle) = 0;
	virtual Mesh* GetMesh();
	virtual Material* GetMaterial();
	virtual Material* GetCopyMaterial();
	virtual Math::Vector3 GetMeshDimension() const;
	AABB GetBoundingBox() const;

	AssetHandle mMesh;
	Material mMaterial;
	BlendState mBlendState;
    float mBumpFactor;
	float mMetallicFactor;
    float mRoughnessFactor;
    Color mEmissiveColor;
	bool mPerspective;
	bool mReceiveShadows;
};
