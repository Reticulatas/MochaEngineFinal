#include "stdafx.h"
#include "ShaderManager.h"

Shader* ShaderManager::mShaderContainer[] = { 0 };
Shader::ShaderType ShaderManager::mShaderCurrentlyBound = Shader::UNASSIGNED;

void ShaderManager::Init()
{
  AssetManager* assetMgr = AssetManager::getInstance();

  mShaderContainer[Shader::DEFERRED_GEOMETRY] = assetMgr->GetAsset("MochaDeferredGeometry.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::DEFERRED_LIGHT]    = assetMgr->GetAsset("MochaDeferredLight.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::ANIMATION]         = assetMgr->GetAsset("MochaAnimation.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::DEBUG]             = assetMgr->GetAsset("MochaDebug.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FORWARD_RENDER]    = assetMgr->GetAsset("MochaForwardRender.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::DIFFUSE_RENDER]    = assetMgr->GetAsset("MochaDiffuseRender.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::POSTPROCESS]       = assetMgr->GetAsset("MochaAddition.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_BLOOM]          = assetMgr->GetAsset("MochaBloom.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::COPY_RESIZE]       = assetMgr->GetAsset("MochaCopyResize.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_GRAINY]         = assetMgr->GetAsset("MochaGrainy.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_PIXELATE]       = assetMgr->GetAsset("MochaCensor.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_MOTIONBLUR]     = assetMgr->GetAsset("MochaMotionBlur.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_CENSORX]        = assetMgr->GetAsset("MochaCensorX.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_CENSORY]        = assetMgr->GetAsset("MochaCensorY.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_BLURX]          = assetMgr->GetAsset("MochaBlurX.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_BLURY]          = assetMgr->GetAsset("MochaBlurY.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_CHROMATIC]	    = assetMgr->GetAsset("MochaChromatic.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_ARTIFACTS]      = assetMgr->GetAsset("MochaArtifacts.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::PARTICLE_SHADER]   = assetMgr->GetAsset("MochaParticleRenderTest.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::TONE_MAPPING]	    = assetMgr->GetAsset("MochaToneMapping.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::DISTORTION_RENDER] = assetMgr->GetAsset("MochaDistortion.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_TRIANGLE]       = assetMgr->GetAsset("MochaTriangle.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FXAA]              = assetMgr->GetAsset("MochaFXAA.ms").GetAssetPtr<Shader>();
  mShaderContainer[Shader::FX_LENSFLARE]		= assetMgr->GetAsset("MochaLensFlare.ms").GetAssetPtr<Shader>();

  for (int i = 0; i < (int)Shader::TOTAL; ++i)
    mShaderContainer[i]->SetShaderType((Shader::ShaderType)i);
}

void ShaderManager::Bind(Shader::ShaderType shaderType)
{
  if (shaderType != mShaderCurrentlyBound)
  {
    mShaderCurrentlyBound = shaderType;
    ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();

	//  sub shaders if available and if the mask is enabled
	/// PIXEL SHADER
	SubShader<ID3D11PixelShader>* ps = mShaderContainer[shaderType]->GetPixelShader();
	if (ps)
		deviceContext->PSSetShader(ps->GetSubShader(), 0, 0);
	else
		deviceContext->PSSetShader(0, 0, 0);
	/// VERTEX SHADER
	SubShader<ID3D11VertexShader>* vs = mShaderContainer[shaderType]->GetVertexShader();
	if (vs)
		deviceContext->VSSetShader(vs->GetSubShader(), 0, 0);
	else
		deviceContext->VSSetShader(0, 0, 0);

	/// GEOMETRY SHADER
	SubShader<ID3D11GeometryShader>* gs = mShaderContainer[shaderType]->GetGeometryShader();
	if (gs)
		deviceContext->GSSetShader(gs->GetSubShader(), 0, 0);
	else
		deviceContext->GSSetShader(0, 0, 0);

	/// COMPUTE SHADER
	SubShader<ID3D11ComputeShader>* cs = mShaderContainer[shaderType]->GetComputeShader();
	if (cs)
		deviceContext->CSSetShader(cs->GetSubShader(), 0, 0);
	else
		deviceContext->CSSetShader(0, 0, 0);



  }
}

void ShaderManager::EnableSubShaders(Shader::ShaderType shader, unsigned char subshaderMask /*= 255*/)
{
	if (shader < Shader::TOTAL)
		mShaderContainer[shader]->EnableSubShaders(subshaderMask);
}

void ShaderManager::DisableSubShaders(Shader::ShaderType shader, unsigned char subshaderMask /*= 255*/)
{
	if (shader < Shader::TOTAL)
		mShaderContainer[shader]->DisableSubShaders(subshaderMask);
}

void ShaderManager::Clear()
{
  mShaderCurrentlyBound = Shader::UNASSIGNED;
  ID3D11DeviceContext* deviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
  deviceContext->PSSetShader(0, 0, 0);
  deviceContext->VSSetShader(0, 0, 0);
  deviceContext->GSSetShader(0, 0, 0);
  deviceContext->CSSetShader(0, 0, 0);
}
