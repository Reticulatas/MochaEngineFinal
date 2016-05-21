#ifndef FBXEXPORTER_H_
#define FBXEXPORTER_H_

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <set>
#include "FBXData.h"
#include "FBXConversion.h"
#include "ChunkFileWriter.h"
#include "RoasterUtility.h"

const unsigned MAXSKINWEIGHTS = 4;

class FBXExporter
{
public:

  FBXExporter();
  ~FBXExporter();

  bool Initialize();
  void Update();
  void LoadFBX(BoostPath fileName);
  bool ExportScene(BoostPath& outPath);

  static FBXExporter* getInstance();

  std::vector<Vector3>& GetUniqueVertices(){ return mUniqueVertices; }
  std::vector<unsigned>& GetUniqueIndices(){ return mUniqueIndices; }
  std::vector<FBXTriangle>& GetUniqueTriangle(){ return mUniqueTriangles; }
private:

  void ProcessGeometry(FbxNode* fbxNode);
  void ProcessSkeletonHierarchy(FbxNode* fbxNode);
  void ReIndexBuffer();
  void ProcessBindPose();
  void ProcessAnimations(unsigned animStackIndex);
  
  void ProcessControlPoints(FbxNode* fbxNode);
  void ProcessMesh(FbxNode* fbxNode);  
  void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outNormal);
  void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, Vector2& outUV);
  void ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outTangent);
  void ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outBinormal);

  void AssociateMaterialToMesh(FbxNode* fbxNode);
  void ProcessMaterials(FbxNode* fbxNode);
  void ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex);
  void ProcessMaterialTexture(FbxSurfaceMaterial* inMaterial, FBXMaterial* ioMaterial);
  
  template<typename T>
  int FindVertex(const T& inTargetVertex, const std::vector<T>& uniqueVertices);

  void ProcessSkeletonHierarchyRecursively(FbxNode* fbxNode, int inDepth, int myIndex, int inParentIndex);
  void ProcessSkinWeights(FbxNode* fbxNode);
  void RemoveSkinWeights(std::vector<BlendingIndexWeightPair>& skinWeights);
  unsigned FindBoneIndexUsingName(std::string inJointName);
  
  void ProcessAnimationTracks(Animation* anim, FbxNode* animNode, FbxAnimStack* animStack, FbxTime start, FbxTime stop, unsigned trackIndex);
  void ExtractKeyTimes(std::set<FbxTime>& keyTimes, FbxAnimLayer* currAnimLayer, FbxProperty& attribute, const char* curveName);
  
  void ExportMesh(ChunkFileWriter& writer);
  void ExportSkeleton(ChunkFileWriter& writer);
  void ExportAnimations(ChunkFileWriter& writer, std::string animName);

  void DestroyScene();
  
  FbxManager* mFbxManager;
  FbxScene* mFbxScene;
  FbxImporter* mFBXImporter;
  FbxPose * mBindPose;
  Converter mConverter;
  bool mHasSkeleton;

  std::unordered_map<unsigned int, ControlPoint*> mControlPoints;
  std::vector<FbxNode*> mBoneNodes;

  // Model's Info
  MeshType mMeshType;
  std::vector<FBXVertex> mFBXVertices;
  std::vector<FBXVertexSkin> mFBXSkinnedVertices;
  std::vector<unsigned> mFBXIndices;
  std::vector<FBXTriangle> mFBXTriangles;
  std::unordered_map<unsigned int, FBXMaterial*> mFBXMaterialLookUp;
  Skeleton mFBXSkeleton;
  std::vector<Animation*> mFBXAnimation;

  // For Physics
  Vector3 mMax, mMin, mOrigin;
  std::vector<Vector3> mUniqueVertices;
  std::vector<unsigned> mUniqueIndices;
  std::vector<FBXTriangle> mUniqueTriangles;
};

#endif