#include "stdafx.h"
#include "CFilmGrain.h"
#include "ConstantBuffer.h"
#include "ShaderManager.h"

CFilmGrain::CFilmGrain() : CPostProcess()
{
}

CFilmGrain::~CFilmGrain()
{

}

void CFilmGrain::OnFree(void)
{
}

void CFilmGrain::OnInit(void)
{
	mBarHeight		= 6;
	mBarSpeed		= 2;
	mBarOverflow	= 1.2f;
	mNoiseDensity	= .5f;
	mPixelDensity	= 2500;
}

void CFilmGrain::OnStart(void)
{
  CPostProcess::OnStart();
}

void CFilmGrain::OnUpdate(void)
{

}


void CFilmGrain::DoPostProcess(const CCamera* cam)
{
	Renderer* renderer = Renderer::getInstance();

	Vector4 bufferData1(renderer->GetOutputFXBuffer()->GetRenderTargetWidth(), renderer->GetOutputFXBuffer()->GetRenderTargetHeight(), FrameController::getInstance()->GetGlobalTime(), mBarHeight);
	Vector4 bufferData2(mBarSpeed, mBarOverflow, mNoiseDensity, mPixelDensity);
	
	
	ConstantBufferManager::BindFXBuffer(bufferData1, bufferData2);
	
	renderer->PostProcess(renderer->GetInputFXBuffer(), NULL, renderer->GetOutputFXBuffer(), Shader::FX_GRAINY);
	renderer->SwapTempFXBuffers();
	renderer->PostProcess(renderer->GetInputFXBuffer(), NULL, renderer->GetOutputFXBuffer(), Shader::COPY_RESIZE);
	renderer->SwapTempFXBuffers();
}

meta_define(CFilmGrain);
___COMP_REGISTER(CFilmGrain);
