#pragma once

#include "ChunkReader.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "RoasterUtility.h"
#include "ConvexHullPhysics.h"

class ModelLoader
{
public:

  void FirstLoad();
  void OnReload();

  // Could be bean file or anim file
  bool LoadBean(Mesh** pMesh, const BoostPath& beanFile);
  ConvexHullMesh* LoadHull(const BoostPath& beanFile);
  Animation* LoadAnim(const BoostPath& beanFile);
  static ModelLoader* getInstance();

private:

  void ImportMesh(Mesh* pMesh, ChunkReader& reader);
  void ImportSkeleton(Mesh* pMesh, ChunkReader& reader);
  void ImportAnimation(Animation* pAnim, ChunkReader& reader);

  
};
