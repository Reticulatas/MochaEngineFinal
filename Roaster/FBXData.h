#ifndef FBXDATA_H
#define FBXDATA_H

#include <fbxsdk.h>
#include "Math/VQS.h"

using namespace Math;

enum MeshType
{
  MESH_STATIC,
  MESH_SKINNED,
  ONLY_ANIMATION
};

struct FBXVertex
{
  Vector3 position;
  Vector4 color;
  Vector2 texcoords;
  Vector3 normal;
  Vector3 tangent;
  Vector3 binormal;

  bool operator==(const FBXVertex& rhs) const
  {
    return (position == rhs.position) &&
      (texcoords == rhs.texcoords) &&
      (normal == rhs.normal) &&
      (tangent == rhs.tangent) &&
      (binormal == rhs.binormal);
  }
};

struct FBXVertexSkin
{
  Vector3 position;
  Vector4 color;
  Vector2 texcoords;
  Vector3 normal;
  Vector3 tangent;
  Vector3 binormal;
  Vector4 blendWeights;
  unsigned blendIndices[4];

  FBXVertexSkin()
  {
    for (unsigned i = 0; i < 4; ++i)
      blendIndices[i] = 0;
  }

  bool operator==(const FBXVertexSkin& rhs) const
  {
    // Each vertex should only have 4 index-weight blending info pairs
    for (unsigned int i = 0; i < 4; ++i)
    {
      if (blendIndices[i] != rhs.blendIndices[i])
        return false;
    }

    return (position == rhs.position) &&
      (texcoords == rhs.texcoords) &&
      (normal == rhs.normal) &&
      (tangent == rhs.tangent) &&
      (binormal == rhs.binormal) &&
      (blendWeights == rhs.blendWeights);
  }
};

struct BlendingIndexWeightPair
{
  unsigned mBlendingIndex;
  float mBlendingWeight;

  BlendingIndexWeightPair() : 
    mBlendingIndex(0),
    mBlendingWeight(0)
  {}

  bool operator < (const BlendingIndexWeightPair& rhs)
  {
    return (mBlendingWeight > rhs.mBlendingWeight);
  }
};

struct ControlPoint
{
  Math::Vector3 mPosition;
  std::vector<BlendingIndexWeightPair> mBlendingInfo;

  ControlPoint()
  {
    mBlendingInfo.reserve(4);
  }
};

struct FBXTriangle
{
  std::vector<unsigned> mIndices;
  std::string mMaterialName;
  unsigned int mMaterialIndex;

  FBXTriangle()
  {
    mMaterialIndex = 0;
  }

  bool operator<(const FBXTriangle& rhs)
  {
    return mMaterialIndex < rhs.mMaterialIndex;
  }
};

struct FBXMaterial
{
  std::string mName;
  Vector3 mAmbient;
  Vector3 mDiffuse;
  Vector3 mEmissive;
  double mTransparencyFactor;
  std::string mDiffuseMapName;
  std::string mEmissiveMapName;
  std::string mGlossMapName;
  std::string mNormalMapName;
  std::string mSpecularMapName;
};

struct LambertMaterial : public FBXMaterial
{

};

struct PhongMaterial : public FBXMaterial
{
  Vector3 mSpecular;
  Vector3 mReflection;
  double mSpecularPower;
  double mShininess;
  double mReflectionFactor;
};

struct Bone
{
  std::string mName;
  int mParentIndex;
  VQS mBindTransform;
  VQS mBoneSpaceTransform;

  Bone()
  {
    mParentIndex = -1;
  }
};

struct Skeleton
{
  std::vector<Bone> mBones;
};

struct KeyFrame
{
  float Time;
  VQS LocalTransform;

  KeyFrame()
  {
    Time = 0;
  }
};

struct Track
{
  std::vector<KeyFrame> mKeyFrames;
};

struct Animation
{
  std::string mName;
  float mDuration;
  std::vector<Track> mTracks;
};

#endif