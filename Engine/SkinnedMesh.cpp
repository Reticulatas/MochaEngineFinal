#include "stdafx.h"
#include "SkinnedMesh.h"
#include "ModelLoader.h"
#include "RoasterUtility.h"

SkinnedMesh::SkinnedMesh(std::string Name, bool isDynamic) : Mesh(Name, isDynamic)
{
  mSkeleton = 0;
  mMeshType = MESH_SKINNED;
}

SkinnedMesh::~SkinnedMesh()
{

}

void SkinnedMesh::BindMesh(ID3D11DeviceContext* context)
{
  unsigned stride = sizeof(SkinnedVertex);
  unsigned offset = 0;
  context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
  context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void SkinnedMesh::DrawMesh(ID3D11DeviceContext* context)
{
  context->DrawIndexed(mIndexCount, 0, 0);
}

bool SkinnedMesh::GenerateBuffers(ID3D11Device* device)
{
  if (!mSkinnedVertices.size() || !mIndices.size())
    return false;

  /* Create vertex buffer */
  D3D11_BUFFER_DESC vbd;
  vbd.ByteWidth = sizeof(SkinnedVertex) * mSkinnedVertices.size();

  if (mBufferDynamic)
  {
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  }
  else
  {
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.CPUAccessFlags = 0;
  }

  vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vbd.MiscFlags = 0;
  vbd.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA vinitData;
  vinitData.pSysMem = mSkinnedVertices.data();
  HR(device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));

  /* Create index buffer */
  D3D11_BUFFER_DESC ibd;
  ibd.ByteWidth = sizeof(UINT) * this->mIndexCount;
  if (mBufferDynamic)
  {
    ibd.Usage = D3D11_USAGE_DYNAMIC;
    ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  }
  else
  {
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.CPUAccessFlags = 0;
  }
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibd.MiscFlags = 0;
  ibd.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA iinitData;
  iinitData.pSysMem = this->mIndices.data();
  HR(device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer));

  return true;
}

void SkinnedMesh::UpdateBuffer(ID3D11Device* device)
{
  ID3D11DeviceContext* context;
  device->GetImmediateContext(&context);

  // Update VERTEX BUFFER
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HR(context->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

  // Get a pointer to the data in the vertex buffer.
  SkinnedVertex* verticesPtr = (SkinnedVertex*)mappedResource.pData;

  // Copy the data into the vertex buffer.
  memcpy(verticesPtr, (void*)mSkinnedVertices.data(), (sizeof(SkinnedVertex) * mSkinnedVertices.size()));

  // Unlock the vertex buffer.
  context->Unmap(mVertexBuffer, 0);


  /************************************************************************/
  /* UPDATE INDEX BUFFERS                                                 */
  /************************************************************************/

  HR(context->Map(mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

  // Get a pointer to the data in the vertex buffer.
  unsigned* indicesPtr = (unsigned*)mappedResource.pData;

  // Copy the data into the vertex buffer.
  memcpy(indicesPtr, (void*)mIndices.data(), (sizeof(unsigned) * mIndices.size()));

  // Unlock the vertex buffer.
  context->Unmap(mIndexBuffer, 0);
}

std::vector<SkinnedVertex>& SkinnedMesh::GetSkinnedVertices()
{
  return mSkinnedVertices;
}

Skeleton* SkinnedMesh::GetSkeleton()
{
  return mSkeleton;
}

