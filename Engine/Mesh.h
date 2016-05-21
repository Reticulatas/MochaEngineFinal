/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once

 
#include "Asset.h"
#include "Vertex.h"
#include "AABB.h"
enum MeshType
{
  MESH_STATIC,
  MESH_SKINNED,
  MESH_LINE
};

class Mesh : public Asset
{

protected:

  /* Vertex and Index Buffers */
  ID3D11Buffer* mVertexBuffer;	// D3D11 Buffer of vertices
  ID3D11Buffer* mIndexBuffer;		// D3D11 Index Buffer

  std::vector<Vertex> mVertices;		// Array of vertices
  std::vector<int> mIndices;			// Array of indices that will determine how the triangles will be drawn. Must be a multiple of 3

  unsigned int mIndexCount;		// Number of Indices stored - This will be passed to DX11 as the IndexCount to be drawn
  unsigned int mVertexCount;		// Number of Vertices - not necessarily the number of vertices that will be rendered
  unsigned int mTriangleCount;
  bool mBufferDynamic;			// If buffers are dynamic we can update them
  
  PrimitiveType mPrimitiveType;	// Default to Triangle List
  MeshType mMeshType;

  // For Physics
  AABB mMeshAABB; // Max and min points of mesh
  Vector3 mOrigin;  // Origin from Maya in case it is not centered to (0,0,0)
  std::vector<Vector3> mUnqiueVertices;
  std::vector<int> mUniqueIndices;

public:

  virtual void BindMesh(ID3D11DeviceContext* context);		// Bind mesh to the pipeline
  virtual void DrawMesh(ID3D11DeviceContext* context);		// Draw da mesh!

  virtual bool GenerateBuffers(ID3D11Device*  device);		// Create Vertex and Index buffer
  virtual void UpdateBuffer(ID3D11Device* device);

  void BeginMesh();
  void EndMesh(ID3D11Device*  device);
  void AddVertex(const Vertex &v);							// Add Another vertex to the mesh (Shouldn't be called at run time, only during loading)
  void AddIndex(unsigned int index);

  void ComputeNormal();

  // Gettors / Settors
  AABB GetAABB(void) { return mMeshAABB; } const
  Vector3 GetOrigin() const { return mOrigin; }
  void SetMeshName(std::string name);
  std::string GetMeshName();  
  PrimitiveType GetPrimitiveType();  
  std::vector<Vertex>& GetVertices() { return mVertices; }
  std::vector<int>& GetIndices() { return mIndices; }
  std::vector<Vector3>& GetUniqueVertices() { return mUnqiueVertices; }
  std::vector<int>& GetUnqiueIndices() { return mUniqueIndices; }
  MeshType GetMeshType() const { return mMeshType; }
  unsigned GetVertexCount() const { return mVertexCount; }
  Mesh(std::string mName = "",bool isDynamic = false);
  virtual ~Mesh();

  virtual void OnAssetFileChanged();

  friend class ModelLoader;



  ___SERIALIZE_BEGIN___
  ___DEFBASE(Asset)
  //___DEFSER(mVertexBuffer, 1)
  ___DEFSER(mVertexCount, 1)
  //___DEFSER(mIndexBuffer, 1)
  ___DEFSER(mIndexCount, 1)
  ___DEFSER(mTriangleCount, 1)
  ___DEFSER(mBufferDynamic, 1)
  ___DEFSER(mVertices, 1)
  ___DEFSER(mIndices, 1)
  ___DEFSER(mPrimitiveType, 1)
  ___SERIALIZE_END___



};
___SERIALIZE_CLASS(Mesh, 1)

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change