#include "stdafx.h"
#include "ConstantBuffer.h"
#include "CParticleEmitter.h"
#include "CParticleRenderer.h"
#include "CShadowProbe.h"

ConstantBuffer ConstantBufferManager::mConstBufferContainer[] = { 0 };

unsigned ConstantBufferManager::mBuffersInUse = 0;

void ConstantBufferManager::Init(ID3D11Device* device)
{
  //mConstBufferContainer[PER_CAMERA] = new ConstantBuffer();
  mConstBufferContainer[PER_CAMERA].CreateBuffer<ConstantBuffer::PerCameraBuffer>(device);
  mConstBufferContainer[PER_MESH].CreateBuffer<ConstantBuffer::PerMeshBuffer>(device);
  mConstBufferContainer[PER_LIGHT].CreateBuffer<ConstantBuffer::PerLightBuffer>(device);
  mConstBufferContainer[PER_ANIM].CreateBuffer<ConstantBuffer::PerAnimBuffer>(device);
  mConstBufferContainer[POST_PROCESS].CreateBuffer<ConstantBuffer::PostProcessBuffer>(device);
  mConstBufferContainer[PER_MATERIAL].CreateBuffer<ConstantBuffer::PerMatBuffer>(device);
  mConstBufferContainer[FORWARDLIGHT].CreateBuffer<ConstantBuffer::ForwardLightBuffer>(device);
  mConstBufferContainer[REFLECTION_PROBE].CreateBuffer<ConstantBuffer::ReflectionProbeBuffer>(device);
  mConstBufferContainer[SHADOW_PROBE].CreateBuffer<ConstantBuffer::ShadowProbeBuffer>(device);
  mConstBufferContainer[PER_MESH_LIGHT].CreateBuffer<ConstantBuffer::PerMeshLight>(device);  
}

void ConstantBufferManager::Destroy()
{
  for (auto& buffer : mConstBufferContainer)
    ReleaseCOM(buffer.mConstBuffer);
}

void ConstantBufferManager::BindCameraBuffer(const Matrix4& wvpMatrix)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::PerCameraBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 0;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[PER_CAMERA].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::PerCameraBuffer*)mappedResource.pData;

  // Copy the matrices into the constant buffer.  
  dataPtr->gWorldViewProj = wvpMatrix;

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[PER_CAMERA].mConstBuffer, 0);

  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[PER_CAMERA].mConstBuffer);
}

void ConstantBufferManager::BindMeshBuffer(Matrix4& worldMatrix, 
                                           Matrix4& worldInvTraspMatrix /*= Matrix4()*/, 
                                           Matrix4& prevWVPMatrix /*= Matrix4()*/
                                           )
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::PerMeshBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 1;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[PER_MESH].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::PerMeshBuffer*)mappedResource.pData;

  // Copy the matrices into the constant buffer.  
  dataPtr->gWorld = worldMatrix;
  dataPtr->gWorldInvTranspose = worldInvTraspMatrix;
  dataPtr->gPrevWVP = prevWVPMatrix;

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[PER_MESH].mConstBuffer, 0);

  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[PER_MESH].mConstBuffer);
}

void ConstantBufferManager::BindLightBuffer(const CLight* light, const CCamera* cam)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::PerLightBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 2;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[PER_LIGHT].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::PerLightBuffer*)mappedResource.pData;

  dataPtr->gInvView = cam->View().Inverted();
  dataPtr->gLight = light->GetLightProperties();
  dataPtr->gEyePos = cam->gameObject->transform->GetGlobalPosition();
  dataPtr->gIntensity = light->GetIntensity();
  // To unproject depth to get back position
  dataPtr->gPlanes.x = cam->Proj()(2, 2);
  dataPtr->gPlanes.y = cam->Proj()(2, 3);

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[PER_LIGHT].mConstBuffer, 0);

  deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[PER_LIGHT].mConstBuffer);
}

void ConstantBufferManager::BindAnimBuffer(std::vector<VQS>& animTransforms)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::PerAnimBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 4;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[PER_ANIM].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::PerAnimBuffer*)mappedResource.pData;

  // Copy the matrices into the constant buffer. 
  for (unsigned i = 0; i < animTransforms.size() && i < MAX_SKELBONES; ++i)
    dataPtr->gAnimTransforms[i] = animTransforms[i].toMatrix();

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[PER_ANIM].mConstBuffer, 0);

  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[PER_ANIM].mConstBuffer);
}

void ConstantBufferManager::BindFXBuffer(const Vector4& data1, const Vector4& data2)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::PostProcessBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 3;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[POST_PROCESS].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::PostProcessBuffer*)mappedResource.pData;
  dataPtr->gDataBank1 = data1;
  dataPtr->gDataBank2 = data2;

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[POST_PROCESS].mConstBuffer, 0);

  deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[POST_PROCESS].mConstBuffer);
}

void ConstantBufferManager::BindMaterialBuffer(const Vector4& tintcolor, const Matrix4& uvTransform)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::PerMatBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 3;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[PER_MATERIAL].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::PerMatBuffer*)mappedResource.pData;
  dataPtr->gTintcolor = tintcolor;
  dataPtr->gUVTransform = uvTransform;

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[PER_MATERIAL].mConstBuffer, 0);

  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[PER_MATERIAL].mConstBuffer);
}

void ConstantBufferManager::BindForwardLightBuffer(const vector<LightProperties>& lightBuffer, const int& numLights, const CCamera* cam)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::ForwardLightBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 2;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[FORWARDLIGHT].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::ForwardLightBuffer*)mappedResource.pData;

  memcpy(dataPtr->gFLight, lightBuffer.data(), sizeof(LightProperties) * lightBuffer.size());
  dataPtr->gFEyePos = cam->gameObject->transform->GetGlobalPosition();
  dataPtr->gNumLights = numLights;

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[FORWARDLIGHT].mConstBuffer, 0);

  deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[FORWARDLIGHT].mConstBuffer);
}

void ConstantBufferManager::BindReflectionProbeBuffer(const Vector3& probePosition, const Vector3& probeDim,float isRenderingProbe /*= 0.0f*/)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBuffer::ReflectionProbeBuffer* dataPtr;
	ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
	unsigned int bufferNumber = 4;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(mConstBufferContainer[REFLECTION_PROBE].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		std::cout << "Error binding constant buffer\n";
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (ConstantBuffer::ReflectionProbeBuffer*)mappedResource.pData;
	dataPtr->gProbePos = ToVector4(probePosition);
	dataPtr->gProbeDim = probeDim;
	dataPtr->isRenderingProbe = isRenderingProbe;

	// Unlock the constant buffer.
	deviceContext->Unmap(mConstBufferContainer[REFLECTION_PROBE].mConstBuffer, 0);

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[REFLECTION_PROBE].mConstBuffer);
}

void ConstantBufferManager::BindShaderProbeBuffer(const CShadowProbe* shadowProbe, bool isRenderingProbe /*= false*/)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  ConstantBuffer::ShadowProbeBuffer* dataPtr;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  unsigned int bufferNumber = 5;

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(mConstBufferContainer[SHADOW_PROBE].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    std::cout << "Error binding constant buffer\n";
    return;
  }

  // Get a pointer to the data in the constant buffer.
  dataPtr = (ConstantBuffer::ShadowProbeBuffer*)mappedResource.pData;
  if (shadowProbe)
  {
    dataPtr->gShadowBuffer.x = shadowProbe->mStrength;
    dataPtr->gShadowBuffer.y = shadowProbe->mBias;
    dataPtr->gShadowBuffer.z = shadowProbe->GetNearZ();
    dataPtr->gShadowBuffer.w = shadowProbe->GetFarZ();
    dataPtr->gUseSoftShadow = (int)shadowProbe->mUseSoftShadows;
  }
  dataPtr->gIsRenderingShadow = (int)isRenderingProbe;

  // Unlock the constant buffer.
  deviceContext->Unmap(mConstBufferContainer[SHADOW_PROBE].mConstBuffer, 0);

  deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[SHADOW_PROBE].mConstBuffer);
}

void ConstantBufferManager::BindMeshLightBuffer(const Vector4& emissiveColor, float bumpFactor, float metallicFactor, float roughnessFactor, const Vector3& camPos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBuffer::PerMeshLight* dataPtr;
	ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
	unsigned int bufferNumber = 3;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(mConstBufferContainer[PER_MESH_LIGHT].mConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		std::cout << "Error binding constant buffer\n";
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (ConstantBuffer::PerMeshLight*)mappedResource.pData;

	dataPtr->gGlobalAmbient = ToVector3(Renderer::getInstance()->mGlobalAmbient);
	dataPtr->gBumpFactor = bumpFactor;
	dataPtr->gEmissiveColor = emissiveColor;
	dataPtr->gMetallicFactor = metallicFactor;
	dataPtr->gRoughnessFactor = roughnessFactor;
	dataPtr->gEmissiveColor = emissiveColor;
	
	dataPtr->gEyePosWorld = ToVector4(camPos, 1);

	// Unlock the constant buffer.
	deviceContext->Unmap(mConstBufferContainer[PER_MESH_LIGHT].mConstBuffer, 0);

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mConstBufferContainer[PER_MESH_LIGHT].mConstBuffer);
}

void ConstantBufferManager::BindParticleUpdateConstantBuffer(const CCamera* cam, const EmitterProperties& eProp, ID3D11Buffer * buf)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	EmitterProperties * dataPtr;
	ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
	unsigned int bufferNumber = 0;

	result = deviceContext->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	
	if (FAILED(result))
	{
		std::cout << "Error binding constant buffer\n";
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (EmitterProperties*)mappedResource.pData;

	*dataPtr = eProp;

    dataPtr->mView = cam->View();
    dataPtr->mViewProj = cam->ProjView();
    dataPtr->mNearPlane = cam->Proj()[2][2];
    dataPtr->mFarPlane = cam->Proj()[2][3];

	// Unlock the constant buffer.
	deviceContext->Unmap(buf, 0);

	//Bind and send in constant buffer
	deviceContext->CSSetConstantBuffers(0, 1, &buf);
}

void ConstantBufferManager::BindParticleRenderConstantBuffer(const CCamera* cam, ID3D11Buffer * buf)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ParticleRenderConstants * dataPtr;
    ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
    unsigned int bufferNumber = 0;

    result = deviceContext->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
    {
        std::cout << "Error binding constant buffer\n";
        return;
    }

    // Get a pointer to the data in the constant buffer.
    dataPtr = (ParticleRenderConstants*)mappedResource.pData;

    dataPtr->camPos = cam->GetPosition();
    dataPtr->gView  = cam->View();
    dataPtr->gProjection = cam->Proj();
    //dataPtr->gWorldViewProj = cam->ProjView();
    dataPtr->quadLength = 0.5f;

    // Unlock the constant buffer.
    deviceContext->Unmap(buf, 0);

    //Bind and send in constant buffer into the Geometry shader
    deviceContext->GSSetConstantBuffers(0, 1, &buf);
}

