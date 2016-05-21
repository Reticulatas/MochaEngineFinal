#include "stdafx.h"
#include "GPUBufferManager.h"


GPUBufferManager::GPUBufferManager(void)
	:ISubsystem()
{
#if 0
	// TEST CODE
	std::vector<BufType> g_vBuf0;
	//create input
	g_vBuf0.resize(100);

	for (int i = 0; i < 100; ++i)
	{
		g_vBuf0[i].SetValue(i, (float)i);
	}

	AddComputeShader<BufType>("MochaTest.ms", &g_vBuf0[0], g_vBuf0.size(), sizeof(BufType));
#endif
}
GPUBufferManager::~GPUBufferManager(void)
{
	for (auto &vec : mComputeShaderMap)
	{
		for (auto &vIter : vec.second)
		{
			delete vIter.mGPUShader;
		}
	}
}
GPUBufferManager* GPUBufferManager::getInstance()
{
	static GPUBufferManager* instance = 0;

	if (!instance)
		instance = new GPUBufferManager();

	return instance;
}
void GPUBufferManager::RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext)
{
	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
	ID3D11ShaderResourceView* ppSRVNULL[3] = { NULL, NULL, NULL };


	for (auto &vec : mComputeShaderMap)
	{
		for (auto &vIter : vec.second)
		{
			
			GPUShader * tmp = vIter.mGPUShader;

			pd3dImmediateContext->CSSetShader(tmp->GetComputeShader(), NULL, 0);
			pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &tmp->GetUAVBuffer()->mBufViewer, nullptr);

			// Check for constant buffer
			if (tmp->GetConstantBuffer())
			{
				//Bind and send in constant buffer
			}

			// Dispatching the update to Compute shader
			pd3dImmediateContext->Dispatch(tmp->GetElemCount(), 1, 1);

			// Unbind
			pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, nullptr);
			pd3dImmediateContext->CSSetShaderResources(0, 3, ppSRVNULL);
			pd3dImmediateContext->CSSetConstantBuffers(0, 0, nullptr);
		}


		//D3D11_MAPPED_SUBRESOURCE MappedResource;
		//BufType *p;
		//pd3dImmediateContext->Map(it.second->GetUAVBuffer()->CopyToDebugBuffer(pd3dImmediateContext), 0, D3D11_MAP_READ, 0, &MappedResource);
		//p = reinterpret_cast<BufType*>(MappedResource.pData);

		//memcpy(&it.second->mData[0], p, 100);
		//for (int i = 0; i < 100; ++i)
		//{
		//	//it.second->mData[i =
		//	Log(std::to_string(p[50].i) + " " + std::to_string(p[50].f) + "\n");
		//}
	}


}
bool GPUBufferManager::Tick(void)
{
	ID3D11DeviceContext* pDeviceContext = Renderer::getInstance()->GetD3D11DeviceContext();

	RunComputeShader(pDeviceContext);

	return true;
}
void GPUBufferManager::RemoveComputeShader(std::string name, unsigned objID)
{
	std::vector< GPUShaderGroup >::iterator toRemove =
		std::find(mComputeShaderMap[name].begin(),
		mComputeShaderMap[name].end(),
		objID);

	if (toRemove != mComputeShaderMap[name].end())
		mComputeShaderMap[name].erase(toRemove);
}
meta_define(GPUBufferManager); 
