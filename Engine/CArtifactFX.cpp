#include "stdafx.h"
#include "CArtifactFX.h"
#include "ConstantBuffer.h"
#include "ShaderManager.h"

CArtifactFX::CArtifactFX() : CPostProcess()
{
}

CArtifactFX::~CArtifactFX()
{

}

void CArtifactFX::OnFree(void)
{
}

void CArtifactFX::OnInit(void)
{
  mBlockThreshold = .2f;
  mLineThreshold = .7f;
}

void CArtifactFX::OnStart(void)
{
  CPostProcess::OnStart();
}

void CArtifactFX::OnUpdate(void)
{

}


void CArtifactFX::DoPostProcess(const CCamera* cam)
{
  Renderer* renderer = Renderer::getInstance();

  Vector4 bufferData1(FrameController::getInstance()->GetGlobalTime(), mBlockThreshold, mLineThreshold, 0);
  Vector4 bufferData2(renderer->GetOutputFXBuffer()->GetRenderTargetWidth(), renderer->GetOutputFXBuffer()->GetRenderTargetHeight(), 0, 0);
  ConstantBufferManager::BindFXBuffer(bufferData1, bufferData2);

  RenderTarget* noise = AssetManager::getInstance()->GetAsset("noiseTexture.png").GetAssetPtr<RenderTarget>();
  renderer->PostProcess(renderer->GetInputFXBuffer(), noise, renderer->GetOutputFXBuffer(), Shader::FX_ARTIFACTS);
  renderer->SwapTempFXBuffers();
}

meta_define(CArtifactFX);
___COMP_REGISTER(CArtifactFX);
