#include "stdafx.h"
#include "CLensFlare.h"
#include "ConstantBuffer.h"
#include "ShaderManager.h"

CLensFlare::CLensFlare() : CPostProcess()
{
}

CLensFlare::~CLensFlare()
{

}

void CLensFlare::OnFree(void)
{
}

void CLensFlare::OnInit(void)
{
	mDispersionFactor = .2f;
	mGhostCount = 3;
	mFlareStrenght = 1.0f;
}

void CLensFlare::OnStart(void)
{
  CPostProcess::OnStart();
}

void CLensFlare::OnUpdate(void)
{

}


void CLensFlare::DoPostProcess(const CCamera* cam)
{
	Renderer* renderer = Renderer::getInstance();

	Vector4 bufferData(renderer->GetOutputFXBuffer()->GetRenderTargetWidth(), renderer->GetOutputFXBuffer()->GetRenderTargetHeight(), .3f, 0);
	ConstantBufferManager::BindFXBuffer(bufferData);
	// Bloom Vert Pass
	renderer->PostProcess(renderer->GetInputFXBuffer(), NULL, renderer->GetOutputFXBuffer(), Shader::FX_BLOOM);
	renderer->SwapTempFXBuffers();

	// Down sample - set viewport to half screen size
	renderer->SetViewport(renderer->mHalfScreenVP);
	renderer->PostProcess(renderer->GetInputFXBuffer(), NULL, &renderer->mColorHalf0, Shader::COPY_RESIZE);

	// Lens Flare Pass
	bufferData.x = renderer->mColorHalf0.GetRenderTargetWidth();
	bufferData.y = renderer->mColorHalf0.GetRenderTargetHeight();
	bufferData.z = mDispersionFactor;
	bufferData.w = mGhostCount;
	Vector4 bufferData1(mFlareStrenght, 0,0,0);
	ConstantBufferManager::BindFXBuffer(bufferData, bufferData1);

	if (mDirtyLensMask.Valid())
	{
		renderer->PostProcess(&renderer->mColorHalf0, mDirtyLensMask.GetAssetPtr<RenderTarget>(), &renderer->mColorHalf1, Shader::FX_LENSFLARE);
	}
	else
	{
		renderer->PostProcess(&renderer->mColorHalf0, NULL, &renderer->mColorHalf1, Shader::FX_LENSFLARE);
	}

	for (int i = 0; i < 5; ++i)
	{
		// Blur passs
		renderer->PostProcess(&renderer->mColorHalf1, NULL, &renderer->mColorHalf0, Shader::FX_BLURX);

		//// Blur Horiz Pass
		//// Up sample - set viewport to back to our screen resolution
		renderer->PostProcess(&renderer->mColorHalf0, NULL, &renderer->mColorHalf1, Shader::FX_BLURY);
	}

	renderer->SetViewport(renderer->mOculusVP);
	renderer->PostProcess(&renderer->mColorHalf1, NULL, renderer->GetOutputFXBuffer(), Shader::COPY_RESIZE);

	renderer->SwapTempFXBuffers();
	// Add blurred bloom to light buffer
	renderer->PostProcess(&renderer->mLightAccumBuffer, renderer->GetInputFXBuffer(), renderer->GetOutputFXBuffer(), Shader::POSTPROCESS);

	renderer->SwapTempFXBuffers();
}

meta_define(CLensFlare);
___COMP_REGISTER(CLensFlare);

