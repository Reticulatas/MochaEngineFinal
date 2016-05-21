#pragma  once

#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"

class SkinnedMesh : public Mesh
{
public:

  SkinnedMesh(std::string Name = "", bool isDynamic = false);
  ~SkinnedMesh();

  virtual void BindMesh(ID3D11DeviceContext* context);
  virtual void DrawMesh(ID3D11DeviceContext* context);		
  virtual bool GenerateBuffers(ID3D11Device*  device);
  virtual void UpdateBuffer(ID3D11Device* device);
  

  std::vector<SkinnedVertex>& GetSkinnedVertices();
  Skeleton* GetSkeleton();  

  friend class ModelLoader;

private:

  std::vector<SkinnedVertex> mSkinnedVertices;
  Skeleton* mSkeleton;  
};