#pragma once
#include "GraphicsInclude.h"
#include "MathIncludes.h"
#include "ConstantBuffer.h"


struct GPUBuffer
{
	virtual ~GPUBuffer(void) { };

	virtual void Shutdown(void) = 0;
	virtual void SafeRelease(IUnknown *p) = 0;
	virtual HRESULT GenerateBuffer(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData) = 0;
	virtual HRESULT GenerateDebugBuffer(ID3D11Device* pDevice) = 0;
	virtual ID3D11Buffer* CopyToDebugBuffer(ID3D11DeviceContext* pd3dImmediateContext) = 0;

	//Property
	virtual ID3D11Buffer * GetBuffer(void) = 0;

	ID3D11Buffer * mBuf      = nullptr;
	ID3D11Buffer * mDebugBuf = nullptr;
};

struct ShaderResourceViewBuffer : public GPUBuffer
{
	ShaderResourceViewBuffer(void);
	virtual ~ShaderResourceViewBuffer(void);

	bool Initialise(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData);
	virtual void Shutdown(void);

	virtual void	SafeRelease(IUnknown *p);
	virtual HRESULT GenerateBuffer(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData);
	virtual HRESULT GenerateDebugBuffer(ID3D11Device* pDevice);
	HRESULT			GenerateSRVBuffer(ID3D11Device* pDevice);
	virtual ID3D11Buffer* CopyToDebugBuffer(ID3D11DeviceContext* pd3dImmediateContext);
	
	//Property
	virtual ID3D11Buffer * GetBuffer(void);

	ID3D11ShaderResourceView * mBufViewer = nullptr;

}; typedef ShaderResourceViewBuffer SRVBuffer;

struct UnorderedAccessViewBuffer : public GPUBuffer
{
	UnorderedAccessViewBuffer(void);
	virtual ~UnorderedAccessViewBuffer(void);

	bool Initialise(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData);
	virtual void Shutdown(void);

	virtual void    SafeRelease(IUnknown *p);
	virtual HRESULT GenerateBuffer(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData);
	virtual HRESULT GenerateDebugBuffer(ID3D11Device* pDevice);
	HRESULT         GenerateUAVBuffer(ID3D11Device* pDevice);
	virtual ID3D11Buffer* CopyToDebugBuffer(ID3D11DeviceContext* pd3dImmediateContext);

	//Property
	virtual ID3D11Buffer * GetBuffer(void);

	ID3D11UnorderedAccessView * mBufViewer = nullptr;

}; typedef UnorderedAccessViewBuffer UAVBuffer;


struct CompoundGPUBuffer : public GPUBuffer
{
	CompoundGPUBuffer(void);
	virtual ~CompoundGPUBuffer(void);

	bool Initialise(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData);
	virtual void Shutdown(void);

	virtual void    SafeRelease(IUnknown *p);
	virtual HRESULT GenerateBuffer(ID3D11Device* pDevice, unsigned uElementSize, unsigned uCount, void* pInitData);
	virtual HRESULT GenerateDebugBuffer(ID3D11Device* pDevice);
	HRESULT			GenerateSRVBuffer(ID3D11Device* pDevice);
	HRESULT         GenerateUAVBuffer(ID3D11Device* pDevice);
	virtual ID3D11Buffer* CopyToDebugBuffer(ID3D11DeviceContext* pd3dImmediateContext);

	//Property
	virtual ID3D11Buffer * GetBuffer(void);

	ID3D11ShaderResourceView *  mSRVBufViewer = nullptr;
	ID3D11UnorderedAccessView * mUAVBufViewer = nullptr;

}; typedef UnorderedAccessViewBuffer UAVBuffer;

#if 1
struct BufType
{
	int i;
	float f;

	void SetValue(int pi, float pf)
	{
		i = pi;
		f = pf;
	}
};
#endif

enum GPUShaderType
{
	GPUST_SRV = 0x1,
	GPUST_UAV = 0x2,
	GPUST_COMPOUND = 0x4,
	GPUST_TOTAL = 0x8
};

class GPUShader
{
public:
	GPUShader(std::string computeShader, GPUShaderType type);
	virtual ~GPUShader(void);

	void Initialise(void);
	void Shutdown(void);

	ID3D11ComputeShader* GetComputeShader(void);

	std::string GetComputeShaderFile() const { return mComputeShaderFile; }
	void        SetComputeShaderFile(std::string val) { mComputeShaderFile = val; }

	AssetHandle GetComputeShaderHandle() const { return mComputeShaderHandle; }
	SRVBuffer * GetSRVBuffer() { return &mSRVBuffer; }
	UAVBuffer * GetUAVBuffer() { return &mUAVBuffer; }
	CompoundGPUBuffer * GetCompoundGPUBuffer() { return &mCompoundBuffer; }

	bool GetFoundComputeShader(void) const { return mFoundComputeShader; }
	void SetFoundComputeShader(bool val) { mFoundComputeShader = val; }
	bool GetInitialised(void) const { return mInitialised; }
	void SetInitialised(bool val) { mInitialised = val; }
	unsigned GetElemCount() const { return mElemCount; }
	unsigned GetElemSize() const { return mElemSize; }

	bool GetConstantBuffer() const { return mConstantBuffer; }

	void InitialiseData(BYTE* data, unsigned elemCount, unsigned elemSize = 0);
	void ReinitialiseData(void);
	void ClearData(void);

	template <typename T>
	void InitialiseConstantBuffer(ID3D11Device* pDevice, T* data)
	{
		if (data == nullptr)
			return;

		mConstantBuffer = mCBuffer.CreateBuffer<T>(pDevice);
	}

	std::vector<BYTE> mData;
	ConstantBuffer    mCBuffer;
private:
	void InternalInitialise(void);

	SRVBuffer mSRVBuffer;
	UAVBuffer mUAVBuffer;
	CompoundGPUBuffer mCompoundBuffer;

	GPUShaderType mType;

	
	
	SubShader<ID3D11ComputeShader> * mComputeShader = nullptr;
	AssetHandle                      mComputeShaderHandle = nullptr;
	std::string						 mComputeShaderFile;

	unsigned mElemCount;
	unsigned mElemSize;
	
	bool mFoundComputeShader = false;
	bool mInitialised = false;
	bool mConstantBuffer = false;
	
};