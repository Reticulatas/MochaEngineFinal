#include "stdafx.h"
#include "LineMesh.h"

LineMesh::LineMesh(std::string Name, bool isDynamic) : Mesh(Name, isDynamic)
{
  mMeshType = MESH_LINE;
  mPrimitiveType = PrimitiveType::LINE_LIST;
  
}

LineMesh::~LineMesh()
{

}

void LineMesh::BindMesh(ID3D11DeviceContext* context)
{
  unsigned stride = sizeof(LineVertex);
  unsigned offset = 0;
  context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
  context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
}

void LineMesh::DrawMesh(ID3D11DeviceContext* context)
{
  context->Draw(mLineVertices.size(), 0);
}

bool LineMesh::GenerateBuffers(ID3D11Device* device)
{
  if (!mLineVertices.size())
    return false;

  /* Create vertex buffer */
  D3D11_BUFFER_DESC vbd;
  vbd.ByteWidth = sizeof(LineVertex) * mLineVertices.size();

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
  vinitData.pSysMem = mLineVertices.data();
  HR(device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));

  return true;
}

void LineMesh::UpdateBuffer(ID3D11Device* device)
{
  ID3D11DeviceContext* context;
  device->GetImmediateContext(&context);

  // Update VERTEX BUFFER
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HR(context->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

  // Get a pointer to the data in the vertex buffer.
  LineVertex* verticesPtr = (LineVertex*)mappedResource.pData;

  // Copy the data into the vertex buffer.
  memcpy(verticesPtr, (void*)mLineVertices.data(), (sizeof(LineVertex) * mLineVertices.size()));

  // Unlock the vertex buffer.
  context->Unmap(mVertexBuffer, 0);
}

std::vector<LineVertex>& LineMesh::GetLineVertices()
{
  return mLineVertices;
}

void LineMesh::AddLineVertex(const LineVertex& v)
{
  mLineVertices.push_back(v);
}

unsigned LineMesh::GetLineCount() const
{
  return mLineVertices.size();
}
