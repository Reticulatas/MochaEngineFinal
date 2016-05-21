#pragma once

#include "GraphicsInclude.h"
#include "MathIncludes.h"
#include "GPUShader.h"
#include <map>


struct GPUShaderGroup
{
	GPUShader *  mGPUShader = nullptr;
	unsigned     mID;
	bool         mManaged = false;
	bool operator==(unsigned objID)
	{
		return mID == objID;
	}

};
class GPUBufferManager : public ISubsystem
{
public:
	GPUBufferManager(void);
	virtual ~GPUBufferManager(void); 

	static GPUBufferManager* getInstance();

	void RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext);


	bool Tick(void) override;

	template <typename T>
	GPUShader * AddComputeShader(std::string name, 
		T* data, 
		GPUShaderType type,
		bool runByGPUMnger,
		unsigned objID,
		unsigned elemCount, 
		unsigned elemSize = 0) 
	{
		if (data == nullptr)
			return nullptr;

		GPUShaderGroup tmp;

		tmp.mGPUShader = new GPUShader(name, type);
		tmp.mGPUShader->Initialise();
		tmp.mGPUShader->InitialiseData((BYTE*)data, elemCount, sizeof(T));

		tmp.mManaged = runByGPUMnger;
		tmp.mID = objID;
		
        if (tmp.mManaged)
		    mComputeShaderMap[name].push_back(tmp);

		return tmp.mGPUShader;
	}

	void RemoveComputeShader(std::string name, unsigned objID);

	std::map<std::string, std::vector<GPUShaderGroup>> mComputeShaderMap;

public:
	metadef(GPUBufferManager)
		endmetadef;

};
