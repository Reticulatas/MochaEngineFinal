#include "stdafx.h"
#include "GPUShader.h"
#include "SubShader.h"
// ********************************* SRV BUFFER START ********************************* //
ShaderResourceViewBuffer::ShaderResourceViewBuffer(void)
	: GPUBuffer()
{

}
ShaderResourceViewBuffer::~ShaderResourceViewBuffer(void)
{
	Shutdown();
}
bool ShaderResourceViewBuffer::Initialise(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData)
{
	if (GenerateBuffer(pDevice, uElementSize, uCount, pInitData)) return false;
	if (GenerateDebugBuffer(pDevice)) return false;
	if (GenerateSRVBuffer(pDevice)) return false;

	return true;
}
void ShaderResourceViewBuffer::Shutdown(void)
{
	SafeRelease(mBuf);
	SafeRelease(mDebugBuf);
	SafeRelease(mBufViewer);
}
void ShaderResourceViewBuffer::SafeRelease(IUnknown *p)
{
	if (p != nullptr)
		p->Release();
}
HRESULT ShaderResourceViewBuffer::GenerateBuffer(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;
	desc.Usage = D3D11_USAGE_DEFAULT;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		HRESULT res = pDevice->CreateBuffer(&desc, &InitData, &mBuf);
		return res;
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, &mBuf);
}
HRESULT ShaderResourceViewBuffer::GenerateDebugBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	mBuf->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	
	return pDevice->CreateBuffer(&desc, NULL, &mDebugBuf);

	//pd3dImmediateContext->CopyResource(debugbuf, mBuf);

	//return debugbuf;
}
HRESULT ShaderResourceViewBuffer::GenerateSRVBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	mBuf->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}
	}

	return pDevice->CreateShaderResourceView(mBuf, &desc, &mBufViewer);
}
ID3D11Buffer* ShaderResourceViewBuffer::CopyToDebugBuffer(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->CopyResource(mDebugBuf, mBuf);

	return mDebugBuf;
}
ID3D11Buffer * ShaderResourceViewBuffer::GetBuffer(void)
{
	return mBuf;
}




// ********************************* SRV BUFFER END ********************************* //
// ********************************* UAV BUFFER START ********************************* //
UnorderedAccessViewBuffer::UnorderedAccessViewBuffer(void)
	: GPUBuffer()
{

}
UnorderedAccessViewBuffer::~UnorderedAccessViewBuffer(void)
{
	Shutdown();
}
bool UnorderedAccessViewBuffer::Initialise(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData)
{
	if (FAILED(GenerateBuffer(pDevice, uElementSize, uCount, pInitData))) return false;
	if (FAILED(GenerateDebugBuffer(pDevice))) return false;
	if (FAILED(GenerateUAVBuffer(pDevice))) return false;

	return true;
}

void UnorderedAccessViewBuffer::Shutdown(void)
{
	SafeRelease(mBuf);
	SafeRelease(mDebugBuf);
	SafeRelease(mBufViewer);
}
void UnorderedAccessViewBuffer::SafeRelease(IUnknown *p)
{
	if (p != nullptr)
		p->Release();
}
HRESULT UnorderedAccessViewBuffer::GenerateBuffer(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;
	desc.Usage = D3D11_USAGE_DEFAULT;
	
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, &mBuf);
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, &mBuf);
}
HRESULT UnorderedAccessViewBuffer::GenerateDebugBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	mBuf->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	return pDevice->CreateBuffer(&desc, NULL, &mDebugBuf);

}
HRESULT UnorderedAccessViewBuffer::GenerateUAVBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	mBuf->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	
	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}
	}

	return pDevice->CreateUnorderedAccessView(mBuf, &desc, &mBufViewer);
}
ID3D11Buffer* UnorderedAccessViewBuffer::CopyToDebugBuffer(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->CopyResource(mDebugBuf, mBuf);

	return mDebugBuf;
}
ID3D11Buffer * UnorderedAccessViewBuffer::GetBuffer(void)
{
	return mBuf;
}

// ********************************* UAV BUFFER END ********************************* //
// ********************************* COMPOUND BUFFER START ********************************* //
CompoundGPUBuffer::CompoundGPUBuffer(void)
	: GPUBuffer()
{

}

CompoundGPUBuffer::~CompoundGPUBuffer(void)
{

}

bool CompoundGPUBuffer::Initialise(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData)
{
	if (FAILED(GenerateBuffer(pDevice, uElementSize, uCount, pInitData))) return false;
	if (FAILED(GenerateDebugBuffer(pDevice))) return false;
	if (FAILED(GenerateSRVBuffer(pDevice))) return false;
	if (FAILED(GenerateUAVBuffer(pDevice))) return false;

	return true;
}

void CompoundGPUBuffer::Shutdown(void)
{

	SafeRelease(mBuf);
	SafeRelease(mDebugBuf);
	SafeRelease(mSRVBufViewer);
	SafeRelease(mUAVBufViewer);

}

void CompoundGPUBuffer::SafeRelease(IUnknown *p)
{
	if (p != nullptr)
		p->Release();
}

HRESULT CompoundGPUBuffer::GenerateBuffer(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;
	desc.Usage = D3D11_USAGE_DEFAULT;
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, &mBuf);
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, &mBuf);
}

HRESULT CompoundGPUBuffer::GenerateDebugBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	mBuf->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	return pDevice->CreateBuffer(&desc, NULL, &mDebugBuf);
}

HRESULT CompoundGPUBuffer::GenerateSRVBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	mBuf->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}
	}

	return pDevice->CreateShaderResourceView(mBuf, &desc, &mSRVBufViewer);
}

HRESULT CompoundGPUBuffer::GenerateUAVBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	mBuf->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}
	}

	return pDevice->CreateUnorderedAccessView(mBuf, &desc, &mUAVBufViewer);
}

ID3D11Buffer* CompoundGPUBuffer::CopyToDebugBuffer(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->CopyResource(mDebugBuf, mBuf);

	return mDebugBuf;
}

ID3D11Buffer * CompoundGPUBuffer::GetBuffer(void)
{
	return mBuf;
}
// ********************************* COMPOUND BUFFER END ********************************* //
// ********************************* GPU SHADER START********************************* //
GPUShader::GPUShader(std::string computeShader, GPUShaderType type)
	:mComputeShaderFile(computeShader)
	, mType(type)
{
}
GPUShader::~GPUShader(void)
{

}
void GPUShader::Initialise(void)
{
	mComputeShaderHandle = AssetManager::getInstance()->GetAsset(mComputeShaderFile);
	
	if (mComputeShaderHandle.Valid())
	{
		mComputeShader = mComputeShaderHandle.GetAssetPtr<Shader>()->GetComputeShader();
	}
	
	//if (!(mComputeShader = AssetManager::getInstance()->GetAsset(name).GetAssetPtr<Shader>()->GetComputeShader()))
	//{
	//	LogVerbose("Failed to find compute shader");
	//}
}
void GPUShader::InternalInitialise(void)
{
	ID3D11Device* pDevice = Renderer::getInstance()->GetD3D11Device();
	ID3D11DeviceContext* pDeviceContext = Renderer::getInstance()->GetD3D11DeviceContext();

	
		// Creating UAV and Shadow resource view buffer, for the shader to 
	if (mType == GPUST_COMPOUND)
		if (!mCompoundBuffer.Initialise(pDevice, mElemSize, mElemCount, &mData[0]))
			return;
	// Creating Shadow resource view buffer, for the shader to read
	else if (mType == GPUST_SRV)
		if (!mSRVBuffer.Initialise(pDevice, mElemSize, mElemCount, &mData[0]))
			return;
	// Creating unordered access buffer, for GPU to output to.
	else if (mType == GPUST_UAV)
		if (!mUAVBuffer.Initialise(pDevice, mElemSize, mElemCount, &mData[0]))
			return;

	//
	mInitialised = true;
}
void GPUShader::Shutdown(void)
{
	mSRVBuffer.Shutdown();
	mUAVBuffer.Shutdown();
	mCompoundBuffer.Shutdown();
	ClearData();

	bool mInitialised = false;
}
void GPUShader::InitialiseData(BYTE* data, unsigned elemCount, unsigned elemSize)
{
	if (data == nullptr || elemCount == 0)
		return;

	// Element size
	if (elemSize == 0)
		mElemSize = elemSize;
	else
		mElemSize = elemSize;

	mElemCount = elemCount;
	
	unsigned totalSize = mElemCount * mElemSize;
	mData.reserve(totalSize);
	//Copying Data to buffer
	BYTE* cpy = reinterpret_cast<BYTE*>(data);

	for (unsigned i = 0; i < totalSize; ++i)
		mData.push_back(cpy[i]);

	// Create buffers if data is valid
	InternalInitialise();
}
void GPUShader::ReinitialiseData(void)
{
	InternalInitialise();
}

void GPUShader::ClearData(void)
{
	mData.clear();
}
ID3D11ComputeShader* GPUShader::GetComputeShader(void)
{
	return mComputeShader->GetSubShader();
}





