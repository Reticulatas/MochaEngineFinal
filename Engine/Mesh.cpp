/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "SupportFunction.h"
void Mesh::BindMesh(ID3D11DeviceContext* context)
{
  /* Set vertex and index buffers */
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
  context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::DrawMesh(ID3D11DeviceContext* context)
{
  context->DrawIndexed(mIndexCount, 0, 0);
}

void Mesh::SetMeshName(std::string name)
{
  this->mName = name;
}

std::string Mesh::GetMeshName()
{
  return mName;
}

bool Mesh::GenerateBuffers(ID3D11Device*  device)
{
  if(!this->mVertices.size() || !this->mIndices.size())
    return false;

  /* Create vertex buffer */
  D3D11_BUFFER_DESC vbd;
  vbd.ByteWidth = sizeof(Vertex) * this->mVertexCount;

  if(mBufferDynamic)
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
  vinitData.pSysMem = this->mVertices.data();
  HR(device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));
  
  /* Create index buffer */
  D3D11_BUFFER_DESC ibd;
  ibd.ByteWidth = sizeof(UINT) * this->mIndexCount;
  if(mBufferDynamic)
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

void Mesh::AddVertex(const Vertex &v)
{
  this->mVertices.push_back(v);
  this->mVertexCount = this->mVertices.size();

}

void Mesh::AddIndex(unsigned int index)
{
  this->mIndices.push_back(index);
  this->mIndexCount = this->mIndices.size();

  if(!(mIndexCount % 3))
    ++mTriangleCount;
}

void Mesh::BeginMesh()
{
  if(!mBufferDynamic)
  {
    if(this->mIndexBuffer)
      ReleaseCOM(mIndexBuffer);

    if(this->mVertexBuffer)
      ReleaseCOM(mVertexBuffer);
  }
  else
  {
    mVertices.clear();
    mIndices.clear();
  }
  mVertexCount = mIndexCount = mTriangleCount = 0;

}

void Mesh::EndMesh(ID3D11Device*  device)
{
  if(!this->mVertexBuffer)
  {
    this->GenerateBuffers(device);
  }
  else
  {
    if(mBufferDynamic)
      this->UpdateBuffer(device);
  }
}

void Mesh::UpdateBuffer(ID3D11Device* device)
{
  ID3D11DeviceContext* context;
  device->GetImmediateContext(&context);

  // Update VERTEX BUFFER
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HR(context->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

  // Get a pointer to the data in the vertex buffer.
  Vertex* verticesPtr = (Vertex*)mappedResource.pData;

  // Copy the data into the vertex buffer.
  memcpy(verticesPtr, (void*)mVertices.data(), (sizeof(Vertex) * mVertices.size()));

  // Unlock the vertex buffer.
  context->Unmap(mVertexBuffer, 0);


  /************************************************************************/
  /* UPDATE INDEX BUFFERS                                                 */
  /************************************************************************/

  HR(context->Map(mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

  // Get a pointer to the data in the vertex buffer.
  unsigned int* indicesPtr = (unsigned int*)mappedResource.pData;

  // Copy the data into the vertex buffer.
  memcpy(indicesPtr, (void*)mIndices.data(), (sizeof(unsigned int) * mIndices.size()));

  // Unlock the vertex buffer.
  context->Unmap(mIndexBuffer, 0);
}

Mesh::Mesh( std::string name /*= 0*/,bool isDynamic /*= false*/ )
{
  this->mVertexBuffer = 0;
  this->mIndexBuffer = 0;
  this->mIndexCount = 0;
  this->mVertexCount = 0;
  this->mTriangleCount = 0;

  this->mPrimitiveType = PrimitiveType::TRIANGLE_LIST;
  mMeshType = MESH_STATIC;

  this->mUseCounter = 0;
  SetMeshName(name);
  mBufferDynamic = isDynamic;
  SetType("Meshes");


}

Mesh::~Mesh()
{
  if(this->mIndexBuffer)
    ReleaseCOM(mIndexBuffer);

  if(this->mVertexBuffer)
    ReleaseCOM(mVertexBuffer);
}

void Mesh::ComputeNormal()
{
  for(UINT i = 0; i < mTriangleCount; ++i)
  {
    UINT i0 = mIndices[i * 3 + 0],
         i1 = mIndices[i * 3 + 1],
         i2 = mIndices[i * 3 + 2];

    Vertex v0 = mVertices[i0],
           v1 = mVertices[i1],
           v2 = mVertices[i2];

    Vector3 a = v1.mPosition - v0.mPosition,
            b = v2.mPosition - v0.mPosition,
            normal = Cross(a, b);

    mVertices[i0].mNormal += normal;
    mVertices[i1].mNormal += normal;
    mVertices[i2].mNormal += normal;
  }

  for(UINT i = 0; i < mVertexCount; ++i)
    mVertices[i].mNormal.Normalize();
    
}

PrimitiveType Mesh::GetPrimitiveType()
{
  return this->mPrimitiveType;
}

void Mesh::OnAssetFileChanged()
{
  Mesh* self = this;
  bool result = ModelLoader::getInstance()->LoadBean(&self, fileID.Filename());

  if (!result)
    Log("Failed to reload " + mName);
  else
    Log("Reloaded " + mName);
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change