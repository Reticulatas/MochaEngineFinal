#include "stdafx.h"
#include "CMotionBlur.h"
#include "ConstantBuffer.h"
#include "ShaderManager.h"

CMotionBlur::CMotionBlur() : CPostProcess()
{
}

CMotionBlur::~CMotionBlur()
{

}

void CMotionBlur::OnFree(void)
{
}

void CMotionBlur::OnInit(void)
{
	mBlurFactor = 1;
}

void CMotionBlur::OnStart(void)
{
  CPostProcess::OnStart();
}

void CMotionBlur::OnUpdate(void)
{

}


void CMotionBlur::DoPostProcess(const CCamera* cam)
{
	Renderer* renderer = Renderer::getInstance();

	Vector4 bufferData(renderer->GetOutputFXBuffer()->GetRenderTargetWidth(), renderer->GetOutputFXBuffer()->GetRenderTargetHeight(), (float)mBlurFactor, 0);
	ConstantBufferManager::BindFXBuffer(bufferData);

	 // Motion blur
	renderer->PostProcess(renderer->GetInputFXBuffer(), &renderer->mVelocityBuffer, renderer->GetOutputFXBuffer(), Shader::FX_MOTIONBLUR);
	renderer->SwapTempFXBuffers();

}

meta_define(CMotionBlur);
___COMP_REGISTER(CMotionBlur);
