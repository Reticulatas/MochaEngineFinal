#include "stdafx.h"
#include "CBloom.h"
#include "ConstantBuffer.h"
#include "ShaderManager.h"

CBloom::CBloom() : CPostProcess()
{
}

CBloom::~CBloom()
{

}

void CBloom::OnFree(void)
{
}

void CBloom::OnInit(void)
{
	mBloomFactor = .9f;
}

void CBloom::OnStart(void)
{
  CPostProcess::OnStart();
}

void CBloom::OnUpdate(void)
{

}


void CBloom::DoPostProcess(const CCamera* cam)
{
	Renderer* renderer = Renderer::getInstance();

	Vector4 bufferData(renderer->GetOutputFXBuffer()->GetRenderTargetWidth(), renderer->GetOutputFXBuffer()->GetRenderTargetHeight(), mBloomFactor, 0);
	ConstantBufferManager::BindFXBuffer(bufferData);
	// Blur Vert Pass
	renderer->PostProcess(renderer->GetInputFXBuffer(), NULL, renderer->GetOutputFXBuffer(), Shader::FX_BLOOM);
	renderer->SwapTempFXBuffers();

	// Down sample - set viewport to half screen size
	renderer->SetViewport(renderer->mHalfScreenVP);
	
	renderer->PostProcess(renderer->GetInputFXBuffer(), NULL, &renderer->mColorHalf0, Shader::FX_BLURX);

	//// Blur Horiz Pass
	//// Up sample - set viewport to back to our screen resolution
	renderer->SetViewport(renderer->mOculusVP);
	bufferData.x = renderer->mColorHalf0.GetRenderTargetWidth();
	bufferData.y = renderer->mColorHalf0.GetRenderTargetHeight();
	ConstantBufferManager::BindFXBuffer(bufferData);
	renderer->PostProcess(&renderer->mColorHalf0, NULL, renderer->GetOutputFXBuffer(), Shader::FX_BLURY);

	renderer->SwapTempFXBuffers();
	// Add blurred bloom to light buffer
	renderer->PostProcess(&renderer->mLightAccumBuffer, renderer->GetInputFXBuffer(), renderer->GetOutputFXBuffer(), Shader::POSTPROCESS);

	renderer->SwapTempFXBuffers();
}

meta_define(CBloom);
___COMP_REGISTER(CBloom);
