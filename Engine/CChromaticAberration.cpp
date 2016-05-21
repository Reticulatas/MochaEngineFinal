#include "stdafx.h"
#include "CChromaticAberration.h"
#include "ConstantBuffer.h"
#include "ShaderManager.h"

CChromaticAberration::CChromaticAberration() : CPostProcess()
{
}

CChromaticAberration::~CChromaticAberration()
{

}

void CChromaticAberration::OnFree(void)
{
}

void CChromaticAberration::OnInit(void)
{

}

void CChromaticAberration::OnStart(void)
{
  CPostProcess::OnStart();
}

void CChromaticAberration::OnUpdate(void)
{

}


void CChromaticAberration::DoPostProcess(const CCamera* cam)
{
	Renderer* renderer = Renderer::getInstance();

	Vector4 bufferData1(renderer->GetOutputFXBuffer()->GetRenderTargetWidth(), renderer->GetOutputFXBuffer()->GetRenderTargetHeight(), FrameController::getInstance()->GetGlobalTime(), 0);
	ConstantBufferManager::BindFXBuffer(bufferData1);
	
	renderer->PostProcess(renderer->GetInputFXBuffer(), NULL, renderer->GetOutputFXBuffer(), Shader::FX_CHROMATIC);
	renderer->SwapTempFXBuffers();
}

meta_define(CChromaticAberration);
___COMP_REGISTER(CChromaticAberration);
