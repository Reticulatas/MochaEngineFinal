#include "stdafx.h"
#include "CCensorFX.h"
#include "ConstantBuffer.h"
#include "ShaderManager.h"

CCensorFX::CCensorFX() : CPostProcess()
{
}

CCensorFX::~CCensorFX()
{

}

void CCensorFX::OnFree(void)
{
}

void CCensorFX::OnInit(void)
{
}

void CCensorFX::OnStart(void)
{
  CPostProcess::OnStart();
}

void CCensorFX::OnUpdate(void)
{

}


void CCensorFX::DoPostProcess(const CCamera* cam)
{
	Renderer* renderer = Renderer::getInstance();

	Vector4 bufferData(renderer->GetOutputFXBuffer()->GetRenderTargetWidth(), renderer->GetOutputFXBuffer()->GetRenderTargetHeight(), 0, 0);
	ConstantBufferManager::BindFXBuffer(bufferData);
	
	renderer->PostProcess(renderer->GetInputFXBuffer(), &renderer->mVelocityBuffer, renderer->GetOutputFXBuffer(), Shader::FX_PIXELATE);
	
	renderer->SwapTempFXBuffers();
	 // Censor FX
	 for (int i = 0; i < 3; ++i)
	 {
	   // Used velocity buffer z component at a mask
	   renderer->PostProcess(renderer->GetInputFXBuffer(), &renderer->mVelocityBuffer, renderer->GetOutputFXBuffer(), Shader::FX_CENSORX);
	   renderer->SwapTempFXBuffers();
	   renderer->PostProcess(renderer->GetInputFXBuffer(), &renderer->mVelocityBuffer, renderer->GetOutputFXBuffer(), Shader::FX_CENSORY);
	   renderer->SwapTempFXBuffers();
	 }
}

meta_define(CCensorFX);
___COMP_REGISTER(CCensorFX);
