#include "stdafx.h"
#include "CTransform.h"
#include "Mesh.h"
#include "AssetManager.h"
#include "EditorSubsystem_Interface.h"
#include "CParticleRenderer.h"
#include "CParticleEmitter.h"
#include "ModelLoader.h"
#include "CAnimationController.h"
#include "MeshFactory.h"
#include "InputManager.h"
#include "EditorSubsystem.h"
#include "EditorState.h"
#include "CDiffuseRenderer.h"
#include "CForwardRenderer.h"
#include "CFontRenderer.h"
#include "ParticleSystem.h"
#include "SupportFunction.h"
#include "SteamViveVR.h"
#include "CShadowProbe.h"

#include <cassert> 
#include <fstream>

/* Singleton */
Renderer* Renderer::getInstance()
{
	static Renderer* instance = 0;

	if (!instance)
		instance = new Renderer();

	return instance;
}

void Renderer::EnableVSync(bool enable)
{
	this->mVSyncOn = enable;
}

bool Renderer::IsVSyncEnabled()
{
	return this->mVSyncOn;
}

ID3D11Device* Renderer::GetD3D11Device()
{
	return this->md3dDevice;
}

ID3D11DeviceContext* Renderer::GetD3D11DeviceContext()
{
	return this->md3dImmediateContext;
}

Renderer::Renderer()
{
	md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;				/* Use Hardware acceleration, duh */
	mEnable4xMsaa = false;									/* Should we use MSAA ? */
	m4xMsaaQuality = 0;

	md3dDevice = 0;
	md3dImmediateContext = 0;
	mSwapChain = 0;
	mOculusSwapChain = 0;

  mGlobalAmbient = Vector4(.0f, .0f, .0f, .0f);

	mVSyncOn = false;
  mEnableFXAA = true;
	mCams = 0;
	mProbes = 0;
	mVR = 0;
	mDebugLinesCount = 0;
}

Renderer::~Renderer()
{
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mOculusSwapChain);

	ReleaseCOM(mAlphaBlendEnabled);
	ReleaseCOM(mAlphaBlendDisabled);
	ReleaseCOM(mDepthWritingEnabled);
	ReleaseCOM(mDepthWritingDisabled);
	ReleaseCOM(mAdditiveBlending);

	ReleaseCOM(mDeferredStecilMask1);
	ReleaseCOM(mDeferredStecilMask2);
	ReleaseCOM(mColorBlendEnabled);

	ReleaseCOM(mPointSampler);
	ReleaseCOM(mAnisotropicSampler);

	ReleaseCOM(mRenderFront);
	ReleaseCOM(mRenderBack);

	// Restore all default settings.
	if (md3dImmediateContext)
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);

	InputLayouts::DestroyAll();
	delete mVR;
}

void Renderer::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mOculusSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	// Resize the swap chain and recreate the render target view.
	unsigned ViewportWidth = Engine::getInstance()->GetGameWnd()->GetViewportWidth();
	unsigned ViewportHeight = Engine::getInstance()->GetGameWnd()->GetViewportHeight();
	unsigned WindowWidth = Engine::getInstance()->GetGameWnd()->GetWindowWidth();
	unsigned WindowHeight = Engine::getInstance()->GetGameWnd()->GetWindowHeight();
	unsigned OculusWndWidth = Engine::getInstance()->GetVRWnd()->GetWindowWidth();
	unsigned OculusWndHeight = Engine::getInstance()->GetVRWnd()->GetWindowHeight();

	ResizeSwapChain(mSwapChain, mFinalRT, Engine::getInstance()->GetGameWnd());
	ResizeSwapChain(mOculusSwapChain, mVRFinalRT, Engine::getInstance()->GetVRWnd());

	if (mVR)
		mVR->Resize();

	// Resize GBuffers
	ResizeRenderTargets();

	//Letter boxing
	mLetterBoxVP.x = Engine::getInstance()->GetGameWnd()->GetScreenOffsetX();
	mLetterBoxVP.y = Engine::getInstance()->GetGameWnd()->GetScreenOffsetY();
	mLetterBoxVP.width = static_cast<float>(ViewportWidth);
	mLetterBoxVP.height = static_cast<float>(ViewportHeight);

	// Set the viewport transform.
	mScreenVP.x = 0;
	mScreenVP.y = 0;
	mScreenVP.width = static_cast<float>(WindowWidth);
	mScreenVP.height = static_cast<float>(WindowHeight);

	mHalfScreenVP.x = 0;
	mHalfScreenVP.y = 0;
	mHalfScreenVP.width = static_cast<float>(OculusWndWidth)* .5f;
	mHalfScreenVP.height = static_cast<float>(OculusWndHeight)* .5f;
	//mHalfScreenVP.width = static_cast<float>(WindowWidth) * .5f;
	//mHalfScreenVP.height = static_cast<float>(WindowHeight) * .5f;

	mCubeMapVP.x = 0;
	mCubeMapVP.y = 0;
  mCubeMapVP.width = static_cast<float>(1024);
	mCubeMapVP.height = static_cast<float>(1024);

	mOculusVP.x = 0;
	mOculusVP.y = 0;
	mOculusVP.width = static_cast<float>(OculusWndWidth);
	mOculusVP.height = static_cast<float>(OculusWndHeight);
	//mOculusVP.width = static_cast<float>(WindowWidth);
	//mOculusVP.height = static_cast<float>(WindowHeight);

	if (mVR && mVR->IsConnected())
	{
		mHalfScreenVP.width = static_cast<float>(mVR->GetRecommendedWidth() * .5f);
		mHalfScreenVP.height = static_cast<float>(mVR->GetRecommendedHeight() * .5f);
		mOculusVP.width = static_cast<float>(mVR->GetRecommendedWidth());
		mOculusVP.height = static_cast<float>(mVR->GetRecommendedHeight());
	}

	SetViewport(mOculusVP);

	// Loops through all states and resizes all cams
	if (GSM::getInstance()->getActiveState())
	{
		GSM::ConstStateIter iter = GSM::getInstance()->EnumerateStates();
		for (; iter != GSM::getInstance()->EnumeratedLastState(); ++iter)
		{
			const std::list<CCamera*>* l = (*iter)->GetListOfComponentsOfType<CCamera>();
			if (l)
			{
				for (auto* cam : *l)
				{
					// Resize projection
					cam->SetProjection(Engine::getInstance()->GetGameWnd()->FixedAspectRatio());
					cam->Resize();
				}
			}
		}
	}
}

void Renderer::ClearFrameBuffer(ID3D11RenderTargetView* rtv, const Color& clearCol)
{
	const float c[4] = {
		clearCol.r,
		clearCol.g,
		clearCol.b,
		clearCol.a };

	md3dImmediateContext->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(c));
}

void Renderer::ClearDepthBuffer(ID3D11DepthStencilView* dsv)
{
  md3dImmediateContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::ClearRenderTargets()
{
	// Set Render Targets to NULL
	std::vector<ID3D11RenderTargetView*> rTargets(mGBuffers.size());
	md3dImmediateContext->OMSetRenderTargets(mGBuffers.size(), rTargets.data(), NULL);
}

void Renderer::ClearShaderResourceView()
{
	// Set Shader Resource View to NULL
	std::vector<ID3D11ShaderResourceView*> sRV(8);
	md3dImmediateContext->PSSetShaderResources(0, sRV.size(), sRV.data());

	//Clear the cache storing the texture bounds to correspondent slots
	ZeroMemory(mBoundTextures.data(), sizeof(ID3D11ShaderResourceView**) * mBoundTextures.size());
}

void Renderer::Clear()
{
	ClearRenderTargets();
	ClearShaderResourceView();
	std::vector<ID3D11Buffer*> buf(5);
  md3dImmediateContext->VSSetConstantBuffers(0, buf.size(), buf.data());
  md3dImmediateContext->PSSetConstantBuffers(0, buf.size(), buf.data());
  ShaderManager::Clear();
}

void Renderer::SwapBuffers()
{
	//HRESULT hr;

	if (mVSyncOn)
	{
		mSwapChain->Present(1, 0);

		// Swap the oculus chain only when it is not connected.
		// Oculus does the swapping internally if connected.
		if (!mVR->IsConnected())
			mOculusSwapChain->Present(1, 0);
	}
	else
	{
		mSwapChain->Present(0, 0);

		if (!mVR->IsConnected())
			mOculusSwapChain->Present(0, 0);
	}

	// if (hr == DXGI_ERROR_DEVICE_REMOVED)
	// {
	//	HRESULT reason = md3dDevice->GetDeviceRemovedReason();
	//
	//	#if defined(_DEBUG)
	//		  wchar_t outString[100];
	//		  size_t size = 100;
	//		  swprintf_s(outString, size, L"Device removed! DXGI_ERROR code: 0x%X\n", reason);
	//		  OutputDebugStringW(outString);
	//	#endif
	// }
}

bool Renderer::Tick()
{
  //block for the oculus last frame render
  //if (OculusVR::getInstance()->OculusTaskDesc())
  //  OculusVR::getInstance()->OculusTaskDesc()->BlockUntilFinished();

  // Render the necessary probes
  RenderProbeList();

	if (Engine::getInstance()->GetIsEditorMode() && !EditorSubsystem::getInstance()->IsTesting())
		mCams = EditorSubsystem::getInstance()->GetEditorState()->GetListOfComponentsOfType<CCamera>();
	else
		mCams = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CCamera>();

	if (!mCams)
		return true;

	// Loop through all cams in the active state
	for (auto camIter : *mCams)
	{
		if (!camIter->isEnabled() || !camIter->GetRenderTarget())
			continue;

		// Renders all NON-oculus cams
		switch (camIter->GetCamType())
		{
		case CCamera::CamType::MAIN:
		{
			RenderScene(camIter);

      ClearFrameBuffer(mFinalRT.GetRenderTargetView(), COLOR::BLACK);
      Rect vp = (Engine::getInstance()->GetIsLetterBox()) ? mLetterBoxVP : mScreenVP;
      FinalImageProcessing(camIter->GetRenderTarget(), &mFinalRT, &vp);
			DrawDebug(mFinalRT, camIter);
			break;
		}

		case CCamera::CamType::GENERIC:
		{
			RenderScene(camIter);
			break;
		}

		case CCamera::CamType::VR:
		{
			// Render to the VR if Rift connected
			if (mVR->IsConnected())
				mVR->RenderVR(camIter);
      
			// Else just render to window
			else
			{
				RenderScene(camIter);
        FinalImageProcessing(camIter->GetRenderTarget(), &mVRFinalRT);
				DrawDebug(mVRFinalRT, camIter);
			}
			break;
		}
		}
	}

	if (Input::GetTriggered(0, "Debug") == 1.f)
		ToggleDebugDraw();

	// Flush Debug lines
	mDebugLinesCount = 0;
  ClearRenderQueues();

	SwapBuffers();

	return true;
}

void Renderer::RenderScene(CCamera* cam)
{
  // Deferred Pipeline
  DrawDeferredGeometry(cam);

  DrawDeferredLighting(cam);
  
  // Handles transparent objects
  DrawForwardRenderer(cam);
  
  // Meshes unaffected by light - Editor GUI
  DrawDiffuseRenderer(cam);
  
  DrawFontRenderer(cam);
  
  DrawPostProcessing(cam);
}

CReflectionProbe* Renderer::GetBestReflectionProbe(CCamera* cam)
{
	if (!cam || !mProbes)
		return 0;

	if (mProbes->size())
	{
		for (auto &probeIter : *mProbes)
		{
			if (IsPointInAABB(cam->gameObject->transform->GetPosition(), probeIter->gameObject->transform->GetPosition(), probeIter->GetBoxDimentions()))
				return probeIter;
		}

		// we have probes, but the camera is not inside any of those
		// return the fisrt probe;
		//return mProbes->front();
	}

	return 0;
}

void Renderer::RenderProbeList(void)
{
  mProbes = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CReflectionProbe>();

  if (mProbes)
  {
    for (auto &probeIter : *mProbes)
    {
      if (probeIter->ShouldRefresh())
        DrawProbe(probeIter);
    }
  }
}

void Renderer::DrawProbe(CCamera* probe)
{
	if (!probe)
		return;
	
  // Save origianl rotation
  Quaternion origRot = probe->gameObject->transform->GetRotationQuaternion();

	for (unsigned i = 0; i < 6; ++i)
	{
    probe->SetupCameraDirection(i);

    // Draw opaque geometry for shadow and reflection probe
    DrawDeferredGeometry(probe);
    // Reflection Probe only
    if (probe->mymeta().getisobjecttype<CReflectionProbe>())
    {
      DrawDeferredLighting(probe);

      SetBlendState(CRenderer::NO_BLEND_DEPTH_LESS);
      SetViewport(mCubeMapVP);
      PostProcess(&mLightAccumBuffer, NULL, probe->GetRenderTarget(), Shader::COPY_RESIZE);
		  ClearRenderQueues();
    }         
	}

  // Place back original position
  probe->gameObject->transform->SetRotation(origRot);
  //probe->gameObject->transform->OnUpdate();
	probe->OnRefreshed();

  if (probe->mymeta().getisobjecttype<CReflectionProbe>())
	  md3dImmediateContext->GenerateMips(probe->GetRenderTarget()->GetShaderResourceView());
}

void Renderer::EarlyZPass(CCamera* cam)
{
  ShaderManager::mShaderContainer[Shader::DEFERRED_GEOMETRY]->DisableSubShaders(PIXEL_SHADER);
  ShaderManager::mShaderContainer[Shader::ANIMATION]->DisableSubShaders(PIXEL_SHADER);

  SetBlendState(CRenderer::NO_BLEND_DEPTH_LESS);

  for (auto& currentElement : mMapDeferredQueues[cam])
  {
    // If shadow probe and mesh is set not to receive shadows - continue
    if (!currentElement->GetMesh() || (!currentElement->IsReceiveShadows() && cam->mymeta().getisobjecttype<CShadowProbe>()))
      continue;

    SetPrimitiveType(currentElement->GetMesh()->GetPrimitiveType());

    currentElement->BindEarlyZBuffers(cam);
    Shader* shader = currentElement->GetMaterial()->GetAttribute<ShaderNode>()->GetShader();
    ShaderManager::Bind(shader->GetShaderType());

    Mesh* elementMesh = currentElement->GetMesh();
    elementMesh->BindMesh(md3dImmediateContext);
    elementMesh->DrawMesh(md3dImmediateContext);
  }

  ShaderManager::mShaderContainer[Shader::DEFERRED_GEOMETRY]->EnableSubShaders(PIXEL_SHADER);
  ShaderManager::mShaderContainer[Shader::ANIMATION]->EnableSubShaders(PIXEL_SHADER);
  ShaderManager::Clear();
}

void Renderer::DrawDeferredGeometry(CCamera* cam)
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

  //CULLING and Selection of what to render here
  //Delete objects marked for deletion
  FrustumCulling(cam);

  RenderTarget* dsView = &mDepthStencilBuffer;
  if (cam->mymeta().getisobjecttype<CShadowProbe>())
  {
    dsView = cam->GetRenderTarget();
    SetViewport(mCubeMapVP);
  }
  else
	  SetViewport(mOculusVP);  
  
  // No render targets, depth pass
  ClearDepthBuffer(dsView->GetDepthStencilView());
  md3dImmediateContext->OMSetRenderTargets(0, 0, dsView->GetDepthStencilView());

  EarlyZPass(cam);

  // Don't need to continue if rendering depth for shadows
  if (cam->mymeta().getisobjecttype<CShadowProbe>())
    return;

	//////////////////////////////////////////////
	//////////////////////////////////////////////
	//////////////////////////////////////////////
	// Static and Skinned Models

  // Clear buffers
  for (unsigned i = 0; i < mGBuffers.size(); ++i)
    ClearFrameBuffer(mGBuffers[i], COLOR::BLACK);

  // Set Render Targets
  md3dImmediateContext->OMSetRenderTargets(mGBuffers.size(), mGBuffers.data(), dsView->GetDepthStencilView());

	//Probes
	CReflectionProbe* bestProbe = 0;
	if (cam->mymeta().getisobjecttype<CCamera>())
	{
		bestProbe = GetBestReflectionProbe(cam);
		if (bestProbe)
		{
			//ConstantBufferManager::BindReflectionProbeBuffer(bestProbe->gameObject->transform->position(), bestProbe->GetBoxDimentions());
			ID3D11ShaderResourceView* const srv = bestProbe->GetRenderTarget()->GetShaderResourceView();
			md3dImmediateContext->PSSetShaderResources(5, 1, &srv);
		}
	}	

	// Render all the MeshRenderers that passed the Frustum Culling
	ProcessRenderQueue<CRenderer>(mMapDeferredQueues[cam], cam);

	ClearRenderTargets();

	// Hack - to unbind Constant Buffers in VS
	std::vector<ID3D11Buffer*> buf(5);
	md3dImmediateContext->VSSetConstantBuffers(0, buf.size(), buf.data());
	md3dImmediateContext->PSSetConstantBuffers(0, buf.size(), buf.data());
}

void Renderer::DrawDeferredLighting(CCamera*cam)
{
	// Render / Update Probes for shadow map
  for (auto* currentElement : mMapLightQueues[cam])
  {
    CShadowProbe* shadowProbe = currentElement->gameObject->GetComponent<CShadowProbe>();
    if (shadowProbe && shadowProbe->IsValid() && shadowProbe->ShouldRefresh())
      DrawProbe(shadowProbe);
  }

	// Must be reset viewport for oculus
	SetViewport(mOculusVP);

	// push GBuffers to the pixel shader
	std::vector<ID3D11ShaderResourceView*> srvs;
	srvs.push_back(mNormalsBuffer.GetShaderResourceView());
	srvs.push_back(mAlbedoBuffer.GetShaderResourceView());
	//srvs.push_back(mSpecularBuffer.GetShaderResourceView());
	srvs.push_back(mDepthStencilBuffer.GetShaderResourceView());

	CReflectionProbe* bestProbe = 0;
	if (cam->mymeta().getisobjecttype<CCamera>())
	{
		bestProbe = GetBestReflectionProbe(cam);
		if (bestProbe)
		{
			ConstantBufferManager::BindReflectionProbeBuffer(bestProbe->gameObject->transform->position(), bestProbe->GetBoxDimentions());
			srvs.push_back(bestProbe->GetRenderTarget()->GetShaderResourceView());
		}
    else
      srvs.push_back(0);
	}
  else if (cam->mymeta().getisobjecttype<CReflectionProbe>())
  {
    //ConstantBufferManager::BindReflectionProbeBuffer(Vector3(0, 0, 0), Vector3(0, 0, 0), 1.0f);
		srvs.push_back(0);  
  }

	md3dImmediateContext->PSSetShaderResources(0, srvs.size(), srvs.data());
	// No need depth stencil for Lighting stage
	md3dImmediateContext->OMSetRenderTargets(1, mLightAccumBuffer.GetRenderTargetViewHandle(), NULL);


	SetBlendState(CRenderer::DEFERRED_BLENDPASS_1);

  // 2 passes 
	for (unsigned i = 0; i < 2; ++i)
	{
    // First Deferred pass doesn't need pixel shader
    if (i == 0)
      ShaderManager::mShaderContainer[Shader::DEFERRED_LIGHT]->DisableSubShaders(PIXEL_SHADER);

		// 2nd pass render to the back and write color
    else
		{
      ShaderManager::mShaderContainer[Shader::DEFERRED_LIGHT]->EnableSubShaders(PIXEL_SHADER);      
			SetRasterState(RENDER_BACK);
			SetBlendState(CRenderer::DEFERRED_BLENDPASS_2);
		}

    ShaderManager::Bind(Shader::DEFERRED_LIGHT);

		// Loop through all the lights
		for (auto* currentElement : mMapLightQueues[cam])
		{    
      // Bind Shadow Map only on second pass
      CShadowProbe* shadowProbe = currentElement->gameObject->GetComponent<CShadowProbe>();
      if (shadowProbe && shadowProbe->IsValid() && i == 1)
      {
        // Texture register 4 is Shadow Map
        md3dImmediateContext->PSSetShaderResources(srvs.size(), 1, shadowProbe->GetRenderTarget()->GetShaderResourceHandle());
        ConstantBufferManager::BindShaderProbeBuffer(shadowProbe, true);        
      }   
      else
        ConstantBufferManager::BindShaderProbeBuffer();

			SetPrimitiveType(currentElement->GetMesh()->GetPrimitiveType());
    
			currentElement->BindBuffers(cam);
    
			ConstantBufferManager::BindLightBuffer(currentElement, cam);
			//if (bestProbe)
			//ConstantBufferManager::BindReflectionProbeBuffer(bestProbe->gameObject->transform->position(), bestProbe->GetBoxDimentions());

			Mesh* elementMesh = currentElement->GetMesh();
    
			elementMesh->BindMesh(md3dImmediateContext);
			elementMesh->DrawMesh(md3dImmediateContext);
		}

    ShaderManager::Clear();
	}

	// Set back to rendering to the front
	SetRasterState(RENDER_FRONT);
	ClearShaderResourceView();

	std::vector<ID3D11Buffer*> buf(5);
	md3dImmediateContext->VSSetConstantBuffers(0, buf.size(), buf.data());
	md3dImmediateContext->PSSetConstantBuffers(0, buf.size(), buf.data());
}

void Renderer::DrawParticles(CCamera* mCam)
{
  md3dImmediateContext->OMSetRenderTargets(1, mLightAccumBuffer.GetRenderTargetViewHandle(), nullptr);
	ParticleSystem::getInstance()->RenderParticles(mCam);
}

void Renderer::DrawPostProcessing(const CCamera* cam)
{
  md3dImmediateContext->IASetInputLayout(InputLayouts::Vert);
  //SetPrimitiveType(PrimitiveType::TRIANGLE_LIST);
	ClearFrameBuffer(GetOutputFXBuffer()->GetRenderTargetView(), COLOR::BLACK);
	ClearFrameBuffer(GetInputFXBuffer()->GetRenderTargetView(), COLOR::BLACK);

  SetBlendState(CRenderer::NO_BLEND_DEPTH_LESS);

	// Copy the light accumuloation buffer to the first output
	// Do initial setup
	//ConstantBufferManager::BindCameraBuffer(cam->PostProcessMatrix());
	PostProcess(&mLightAccumBuffer, NULL, GetOutputFXBuffer(), Shader::COPY_RESIZE);
	SwapTempFXBuffers();
  
	CPostProcess* postproc_buffer[10];
	unsigned found_postprocs = 0;

	try {
		cam->gameObject->GetComponentsFromBase<CPostProcess>(postproc_buffer, 10, &found_postprocs);
	}
	catch (std::exception&)
	{
		Log("No post process effects will display for this frame - too many in scene (>10)");
		return;
	}

	for (unsigned i = 0; i < found_postprocs; ++i)
	{
		// Post process !!!!!!!
		if (postproc_buffer[i]->isEnabled())
			postproc_buffer[i]->DoPostProcess(cam);
	}

	PostProcess(GetInputFXBuffer(), NULL, cam->GetRenderTarget(), Shader::COPY_RESIZE);
	SwapTempFXBuffers();
  

 // // Pixelate Pass
 // texDim = Vector2(mColor0.GetRenderTargetWidth(), mColor0.GetRenderTargetHeight());  
 // ConstantBufferManager::BindFXBuffer(texDim);
 //
 // PostProcess(&mColor0, &mVelocityBuffer, &mColor1, Shader::FX_PIXELATE);
 //
 // // Censor FX
 // for (int i = 0; i < 3; ++i)
 // {
 //   // Used velocity buffer z component at a mask
 //   PostProcess(&mColor1, &mVelocityBuffer, &mColor0, Shader::FX_CENSORX);    
 //   PostProcess(&mColor0, &mVelocityBuffer, &mColor1, Shader::FX_CENSORY);
 // }
 //


  Clear();
}

void Renderer::DrawForwardRenderer(CCamera* cam)
{

  // Bind all lights for forward rendering
  //const std::list<CLight*>* listLight = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CLight>();
  static vector<LightProperties> lightProperties(MAX_FORWARDLIGHTS);
  int i = 0;

  //if (listLight)
  //  for (auto& iter : *listLight)
  for (auto* iter : mMapLightQueues[cam])
  {
      lightProperties[i++] = iter->GetLightProperties();
      if (i == MAX_FORWARDLIGHTS)
          break;
  }

  ConstantBufferManager::BindForwardLightBuffer(lightProperties, i, cam);

  DrawParticles(cam);
  
  // Forward
  if (mMapForwardQueues[cam].empty())
      return;

  md3dImmediateContext->OMSetRenderTargets(1, mLightAccumBuffer.GetRenderTargetViewHandle(), mDepthStencilBuffer.GetDepthStencilView());
  

  std::vector<ID3D11ShaderResourceView*> srvs;
  CReflectionProbe* bestProbe = 0;
  if (!cam->mymeta().getisobjecttype<CReflectionProbe>())
  {
	  bestProbe = GetBestReflectionProbe(cam);
	  if (bestProbe)
	  {
		  ConstantBufferManager::BindReflectionProbeBuffer(bestProbe->gameObject->transform->position(), bestProbe->GetBoxDimentions());
		  srvs.push_back(bestProbe->GetRenderTarget()->GetShaderResourceView());
		  md3dImmediateContext->PSSetShaderResources(5, srvs.size(), srvs.data());
	  }
  }

  // Render all the objects that passed the Frustum Culling
  ProcessRenderQueue(mMapForwardQueues[cam], cam);

  Clear();
}

void Renderer::DrawDiffuseRenderer(CCamera* cam)
{
  if (mMapDiffuseQueues[cam].empty())
    return;

  md3dImmediateContext->OMSetRenderTargets(1, mLightAccumBuffer.GetRenderTargetViewHandle(), mDepthStencilBuffer.GetDepthStencilView());

  // Render all the objects that passed the Frustum Culling
  ProcessRenderQueue(mMapDiffuseQueues[cam], cam);

  Clear();
}

void Renderer::DrawFontRenderer(CCamera* cam)
{
  if (mMapFontQueues[cam].empty())
    return;

  md3dImmediateContext->OMSetRenderTargets(1, mLightAccumBuffer.GetRenderTargetViewHandle(), mDepthStencilBuffer.GetDepthStencilView());

  for (auto& currentElement : mMapFontQueues[cam])
  {
    if (!currentElement->GetMesh() || !currentElement->isEnabled())
      continue;

    SetPrimitiveType(currentElement->GetMesh()->GetPrimitiveType());
    Mesh* elementMesh = currentElement->GetMesh();
    elementMesh->BindMesh(md3dImmediateContext);

    static_cast<CFontRenderer*>(currentElement)->Logic(cam);

    // Unbind Texture Shader Resource Views
    ClearShaderResourceView();
  }

  Clear();
}

void Renderer::PostProcess(RenderTarget* src0, RenderTarget* src1, RenderTarget* dest, Shader::ShaderType shader, bool clearFramebuffer)
{
  if (clearFramebuffer)
    ClearFrameBuffer(dest->GetRenderTargetView(), COLOR::BLACK);

  // Bind shader
  ShaderManager::Bind(shader);
  
  // Instead of a quad, draw on a single triangle
  md3dImmediateContext->VSSetShader(ShaderManager::mShaderContainer[Shader::FX_TRIANGLE]->GetVertexShader()->GetSubShader(), 0, 0);

  md3dImmediateContext->OMSetRenderTargets(1, dest->GetRenderTargetViewHandle(), NULL);

  std::vector<ID3D11ShaderResourceView*> srv;
  srv.emplace_back(src0->GetShaderResourceView());
  if (src1)                 
    srv.emplace_back(src1->GetShaderResourceView());
  md3dImmediateContext->PSSetShaderResources(0, srv.size(), srv.data());

  //Mesh* quad = AssetManager::getInstance()->GetAsset("Quad.mesh").GetAssetPtr<Mesh>();
  //quad->BindMesh(md3dImmediateContext);
  //quad->DrawMesh(md3dImmediateContext);
  SetPrimitiveType(TRIANGLE_STRIP);
  md3dImmediateContext->Draw(4, 0);

  ClearRenderTargets();
  ClearShaderResourceView();
  ShaderManager::Clear();
}

void Renderer::FinalImageProcessing(RenderTarget* src, RenderTarget* dest, Rect* vp)
{
  // Luma for fxaa saved in alpha
  PostProcess(src, NULL, GetOutputFXBuffer(), Shader::TONE_MAPPING);
  SwapTempFXBuffers();

  // FXAA
  if (mEnableFXAA)
  {
    Vector4 bufferData1(1.f / GetInputFXBuffer()->GetRenderTargetWidth(), 1.f / GetInputFXBuffer()->GetRenderTargetHeight(), 0, 0);
    ConstantBufferManager::BindFXBuffer(bufferData1);
    PostProcess(GetInputFXBuffer(), NULL, GetOutputFXBuffer(), Shader::FXAA);
    SwapTempFXBuffers();
  }

  if (vp)
    SetViewport(*vp);

  // Do not need to clear framebuffer since we are overwriting it
  PostProcess(GetInputFXBuffer(), NULL, dest, Shader::COPY_RESIZE, false);
}

/* Init Direct 3D */
bool Renderer::InitD3D()
{
	// Create the device and device context.

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;

  //mFrameBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	mFrameBufferFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
	mDepthBufferFormat = DXGI_FORMAT_R32_TYPELESS;

#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL fLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		md3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		fLevels, 3,              // default feature level array
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}


	switch (featureLevel)
	{
	case D3D_FEATURE_LEVEL_11_0:
		std::cout << "Direct3D Feature Level 11 supported." << std::endl;
		break;
	case D3D_FEATURE_LEVEL_10_1:
		std::cout << "Direct3D Feature Level 10.1 supported." << std::endl;
		break;
	case D3D_FEATURE_LEVEL_10_0:
		std::cout << "Direct3D Feature Level 10 supported." << std::endl;
		break;
	default:
		std::cout << "Nothing is supported!" << std::endl;
		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	if (featureLevel == D3D_FEATURE_LEVEL_11_0 && mEnable4xMsaa)
	{
		HR(md3dDevice->CheckMultisampleQualityLevels(
			GetFrameBufferFormat(), 4, &m4xMsaaQuality));
		assert(m4xMsaaQuality > 0);
	}
	else
	{
		mEnable4xMsaa = false;
		m4xMsaaQuality = 0;
	}

	CreateSwapChain(&mSwapChain, Engine::getInstance()->GetGameWnd());
	CreateSwapChain(&mOculusSwapChain, Engine::getInstance()->GetVRWnd());

	return true;
}

void Renderer::CreateSwapChain(IDXGISwapChain** swapChain, MochaWindow* Wnd)
{
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = Wnd->GetWindowWidth();     // Note: Change to ViewportWidth for Letterboxing
	sd.BufferDesc.Height = Wnd->GetWindowHeight();
  sd.BufferDesc.RefreshRate.Numerator = (Wnd == Engine::getInstance()->GetGameWnd()) ? 60 : 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	sd.BufferCount = 1;										/* Just one for double-buffering */
	sd.OutputWindow = Wnd->GetHWND();
	sd.Windowed = !Wnd->IsFullScreen();
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, swapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
}

void Renderer::ResizeSwapChain(IDXGISwapChain* swapChain, RenderTarget& rt, MochaWindow* hWnd)
{
  // Must release render target first
  rt.ReleaseTarget();
  HR(swapChain->ResizeBuffers(1, hWnd->GetWindowWidth(), hWnd->GetWindowHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0));
  // Getting the Back buffer
  HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(rt.GetTextureHandle())));

  rt.CreateFromTexture();
}

void Renderer::Init(VR* vr)
{
	if (this->InitD3D())
		std::cout << "Direct3D was Initialized correctly!\nMocha Studio ready to render kickass graphics" << std::endl;

  if (vr)
  {
    // Init VR - Steam or Oculus
    mVR = vr;
    //mVR = OculusVR::getInstance();
    bool result = mVR->StartUp();

    // No need to resize -> oculus init already resizes
    if (!result)
      OnResize();
  }

	// Create Render States Description
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	CreateRenderStates();
	CreateSamplerStates();

	currentBlendState = CRenderer::BlendState(- 1);
	SetBlendState(CRenderer::NO_BLEND_DEPTH_LESS);
	EnableAlphaBlending(true);
	EnableDepthWriting(true);
	EnableDebugDraw(false);

	// Render to front - Back face culled
	SetRasterState(RENDER_FRONT);

	// implement initialization here
	SetPrimitiveType(PrimitiveType::TRIANGLE_LIST);	// Default to Triangle List

	ShaderManager::Init();
	InputLayouts::Init(md3dDevice);
	ConstantBufferManager::Init(md3dDevice);

	// Init Debug Lines
	mDebugLines.resize(MAXLINES * 4);

	//Bound Texture Cache
	mBoundTextures.resize(8);

	// Initializes the FX Buffer handles
	mInputFXBuffer = &mColor1;
	mOutputFXBuffer = &mColor0;
}

PrimitiveType Renderer::GetPrimitiveType()
{
	return this->mPrimitiveType;
}

void Renderer::SetPrimitiveType(PrimitiveType type)
{
	if (type != this->mPrimitiveType)
	{
		this->mPrimitiveType = type;
		// Set Primitive 
		md3dImmediateContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)this->mPrimitiveType);
	}
}

void Renderer::FrustumCulling_DoPass(CCamera* mCam, BaseState* state)
{
	// Points of the AABB in world space
	static std::vector<Vector3> verts(8);
	static Vector3 origin, rotatedX, rotatedY, rotatedZ;

	const std::list<CRenderer*>* listRenderers = state->GetListOfComponentsOfType<CRenderer>();

	if (listRenderers)
	{
		// Get vertices in world space
		for (auto& iter : *listRenderers)
		{
			if (!iter->isEnabled() || !iter->mMesh.Valid() || !mCam->gameObject->mLayerMask.IsOnLayer(iter->gameObject->mLayerMask.GetLayerMasks()))
				continue;         

      bool passCulling = false;

      if (iter->mPerspective)
      {
        origin = iter->GetMesh()->GetOrigin();
        iter->GetFrustumCullData(origin, rotatedX, rotatedY, rotatedZ);

        // + + +
        verts[0] = origin + rotatedX + rotatedY + rotatedZ;
        // + + -
        verts[1] = origin + rotatedX + rotatedY - rotatedZ;
        // + - +
        verts[2] = origin + rotatedX - rotatedY + rotatedZ;
        // - + +
        verts[3] = origin - rotatedX + rotatedY + rotatedZ;
        // - - +
        verts[4] = origin - rotatedX - rotatedY + rotatedZ;
        // - + -
        verts[5] = origin - rotatedX + rotatedY - rotatedZ;
        // + - -
        verts[6] = origin + rotatedX - rotatedY - rotatedZ;
        // - - -
        verts[7] = origin - rotatedX - rotatedY - rotatedZ;

        passCulling = mCam->CullAABB(verts);
      }
			
  
			// AABB cull check
      if (!iter->mPerspective || passCulling)
			{
				if (iter->mymeta().getisobjecttype<CMeshRenderer>() || iter->mymeta().getisobjecttype<CSkinMeshRenderer>())
					mMapDeferredQueues[mCam].push_back(iter);

				else if (iter->mymeta().getisobjecttype<CForwardRenderer>())
					mMapForwardQueues[mCam].push_back(iter);

				else if (iter->mymeta().getisobjecttype<CDiffuseRenderer>())
					mMapDiffuseQueues[mCam].push_back(iter);

				else if (iter->mymeta().getisobjecttype<CFontRenderer>())
					mMapFontQueues[mCam].push_back(iter);
			}
		}
	}

	CRenderer::sortBackToFront BackToFront(mCam);
	std::sort(mMapForwardQueues[mCam].begin(), mMapForwardQueues[mCam].end(), BackToFront);
	std::sort(mMapDiffuseQueues[mCam].begin(), mMapDiffuseQueues[mCam].end(), BackToFront);
	std::sort(mMapFontQueues[mCam].begin(), mMapFontQueues[mCam].end(), BackToFront);

	const std::list<CLight*>* listLight = state->GetListOfComponentsOfType<CLight>();
	if (listLight)
	{
		// Iterate through all Lights
		for (auto& light : *listLight)
		{
			if (!light->isEnabled() || !mCam->gameObject->mLayerMask.IsOnLayer(light->gameObject->mLayerMask.GetLayerMasks()))
				continue;

			light->SetLightType(light->mLightType);

			if (light->mLightType == CLight::DIRECTIONAL)
			{
				mMapLightQueues[mCam].push_back(light);
				continue;
			}

			if (mCam->CullSphere(light->gameObject->transform->GetGlobalPosition(), light->GetRange()))
				mMapLightQueues[mCam].push_back(light);
		}
	}

  // Not thread safe
  mCam->mVisibleObjects = mMapDeferredQueues[mCam].size() + mMapForwardQueues[mCam].size() + mMapDiffuseQueues[mCam].size() + mMapFontQueues[mCam].size() + mMapLightQueues[mCam].size();
}

void Renderer::FrustumCulling(CCamera* mCam)
{
	mCam->mVisibleObjects = 0;

	FrustumCulling_DoPass(mCam, GSM::getInstance()->getActiveState());
	if (Engine::getInstance()->GetIsEditorMode() && !EditorSS::IsTesting())
		FrustumCulling_DoPass(mCam, EditorSubsystem::getInstance()->GetEditorState());
}

void Renderer::EnableAlphaBlending(bool enabled)
{
	/* static bool isEnabled = false;
	 if(enabled == isEnabled)
	 return;

	 isEnabled = enabled;*/

	if (enabled)
		md3dImmediateContext->OMSetBlendState(mAlphaBlendEnabled, nullptr, 0xffffff);
	else
		md3dImmediateContext->OMSetBlendState(mAlphaBlendDisabled, nullptr, 0xffffff);
}

void Renderer::EnableDepthWriting(bool enable)
{
	md3dImmediateContext->OMSetDepthStencilState(NULL, 0);

	if (enable)
		md3dImmediateContext->OMSetDepthStencilState(mDepthWritingEnabled, 0);
	else
		md3dImmediateContext->OMSetDepthStencilState(mDepthWritingDisabled, 0);
}

void Renderer::ResizeRenderTargets()
{
	unsigned textureWidth = Engine::getInstance()->GetVRWnd()->GetWindowWidth();
	unsigned textureHeight = Engine::getInstance()->GetVRWnd()->GetWindowHeight();

	if (mVR->IsConnected())
	{
    textureWidth = mVR->GetRecommendedWidth();
    textureHeight = mVR->GetRecommendedHeight();
	}

	// Creating GBuffer Render Targets - Geometry Stage
	mNormalsBuffer.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat());
	mAlbedoBuffer.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat());
	//mSpecularBuffer.InitRenderTarget(textureWidth, textureHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// Creating LightAccumulation RT - Lighting Stage
	mLightAccumBuffer.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat());

	mVelocityBuffer.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat());

	// Depth Stencil Buffer - For Reading the Depth
	mDepthStencilBuffer.InitRenderTargetDepth(textureWidth, textureHeight, DXGI_FORMAT_R32_TYPELESS);

	mGBuffers.clear();
	mGBuffers.emplace_back(mNormalsBuffer.GetRenderTargetView());
	mGBuffers.emplace_back(mAlbedoBuffer.GetRenderTargetView());
	//mGBuffers.emplace_back(mSpecularBuffer.GetRenderTargetView());
	mGBuffers.emplace_back(mVelocityBuffer.GetRenderTargetView());
	mGBuffers.emplace_back(mLightAccumBuffer.GetRenderTargetView());

	// PP-FX Buffers
	mColor0.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat());
	mColor1.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat());
	mColorHalf0.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat(), 1.f / 2);
	mColorHalf1.InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat(), 1.f / 2);
}

void Renderer::CreateRenderStates()
{
	D3D11_RASTERIZER_DESC rasterDesc;

	// Rasterizer for rendering to front (back face culling)
	//rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	md3dDevice->CreateRasterizerState(&rasterDesc, &mRenderFront);

	// Rasterizer for rendering to back (front face culling)
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.DepthClipEnable = false;

	md3dDevice->CreateRasterizerState(&rasterDesc, &mRenderBack);

	/// DEPTH WRITING DISABLED
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace = dsDesc.FrontFace;

	md3dDevice->CreateDepthStencilState(&dsDesc, &mDepthWritingDisabled);

	/// DEPTH WRITING ENABLED
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace = dsDesc.FrontFace;

	//dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	md3dDevice->CreateDepthStencilState(&dsDesc, &mDepthWritingEnabled);

  // Depth Writing - LESS EQUAL
  dsDesc.DepthEnable = TRUE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
  dsDesc.StencilEnable = true;
  dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
  dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
  dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
  dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  dsDesc.BackFace = dsDesc.FrontFace;

  md3dDevice->CreateDepthStencilState(&dsDesc, &mDepthWriting_LESSEQUAL);

	/// 1st pass for deferred stencil masking algorithm
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace = dsDesc.FrontFace;

	md3dDevice->CreateDepthStencilState(&dsDesc, &mDeferredStecilMask1);

	/// 2nd pass for deferred stencil masking algorithm
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.FrontFace = dsDesc.BackFace;

	md3dDevice->CreateDepthStencilState(&dsDesc, &mDeferredStecilMask2);

	// Create an color enabled blend state description. - No alpha
	D3D11_BLEND_DESC colorBlendDesc;
	colorBlendDesc.AlphaToCoverageEnable = 0;
	colorBlendDesc.IndependentBlendEnable = FALSE;

	colorBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	colorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	colorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	colorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	colorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	colorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_BLEND_FACTOR;
	colorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	colorBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	md3dDevice->CreateBlendState(&colorBlendDesc, &mColorBlendEnabled);

	////////////////////////////////////////////ALPHA BLENDING ////////////////////////////////////////////////

	// Alpha Enabled ///////////////////////////
	D3D11_BLEND_DESC oBlendStateDesc;

	oBlendStateDesc.AlphaToCoverageEnable = 0;
	oBlendStateDesc.IndependentBlendEnable = false; //set to false, don't need loop below... but just in case

	oBlendStateDesc.RenderTarget[0].BlendEnable = 1;
	oBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	oBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	oBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    oBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    oBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
  //oBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  //oBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	oBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	oBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the blend state from the description
	md3dDevice->CreateBlendState(&oBlendStateDesc, &mAlphaBlendEnabled);

	// Alpha DISABLED ///////////////////////////

	oBlendStateDesc.AlphaToCoverageEnable = 0;
	oBlendStateDesc.IndependentBlendEnable = false; //set to false, don't need loop below... but just in case

	oBlendStateDesc.RenderTarget[0].BlendEnable = 0;
	oBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	oBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	oBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	oBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	oBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	oBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	oBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the blend state from the description
	md3dDevice->CreateBlendState(&oBlendStateDesc, &mAlphaBlendDisabled);

	// Additive Blending
	//oBlendStateDesc.AlphaToCoverageEnable = 0;
	//oBlendStateDesc.IndependentBlendEnable = 0;
	//oBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	//oBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//oBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	//oBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//oBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//oBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	//oBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    oBlendStateDesc.IndependentBlendEnable = false;
    oBlendStateDesc.AlphaToCoverageEnable = true;
    oBlendStateDesc.RenderTarget[0].BlendEnable = true;
    oBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    oBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    oBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    oBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    oBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    oBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    oBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	md3dDevice->CreateBlendState(&oBlendStateDesc, &mAdditiveBlending);
}

void Renderer::CreateSamplerStates()
{
	D3D11_SAMPLER_DESC samplerDesc;

	// Point Sampler
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	md3dDevice->CreateSamplerState(&samplerDesc, &mPointSampler);


	// Anisotropic Sampler
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	md3dDevice->CreateSamplerState(&samplerDesc, &mAnisotropicSampler);

	// Clamp Anisotropic Sampler
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	md3dDevice->CreateSamplerState(&samplerDesc, &mClampAnisotropicSampler);

  // Clamp Bilinear Sampler
  ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  md3dDevice->CreateSamplerState(&samplerDesc, &mClampBilinearSampler);

	std::vector<ID3D11SamplerState*> samplerStates;
  samplerStates.push_back(mPointSampler);
  samplerStates.push_back(mAnisotropicSampler);
  samplerStates.push_back(mClampAnisotropicSampler);
  samplerStates.push_back(mClampBilinearSampler);
  md3dImmediateContext->PSSetSamplers(0, samplerStates.size(), samplerStates.data());
}

/////////////////////////////////////////////////////////////////////
//////////////////////  DEBUG FUNCTIONS  ////////////////////////////
/////////////////////////////////////////////////////////////////////
void Renderer::DrawLine(const Vector3& begin, const Vector3& end, const Color& col)
{
	if (!mEnableDebugDraw)
		return;
	
	DrawLinePerma(begin, end, col);
}

void Renderer::DrawLinePerma(const Vector3& begin, const Vector3& end, const Color& col)
{
	if (mDebugLinesCount >= mDebugLines.size())
		return;

	mDebugLines[mDebugLinesCount++] = LineVertex(begin, col);
	mDebugLines[mDebugLinesCount++] = LineVertex(end, col);
}

void Renderer::DrawBoxPerma(const Vector3& pos, const Vector3& halfLengths, const Color& col)
{
    if (mDebugLinesCount >= mDebugLines.size())
        return;
	Math::Vector3 pos0 = pos + halfLengths;
	Math::Vector3 pos1 = pos + Vector3(-halfLengths.x, halfLengths.y, halfLengths.z);
	Math::Vector3 pos2 = pos + Vector3(-halfLengths.x, -halfLengths.y, halfLengths.z);
	Math::Vector3 pos3 = pos + Vector3(halfLengths.x, -halfLengths.y, halfLengths.z);

	Math::Vector3 pos4 = pos + Vector3(halfLengths.x, -halfLengths.y, -halfLengths.z);
	Math::Vector3 pos5 = pos + Vector3(-halfLengths.x, -halfLengths.y, -halfLengths.z);
	Math::Vector3 pos6 = pos + Vector3(-halfLengths.x, halfLengths.y, -halfLengths.z);
	Math::Vector3 pos7 = pos + Vector3(halfLengths.x, halfLengths.y, -halfLengths.z);

	Renderer::getInstance()->DrawLinePerma(pos0, pos1, col);
	Renderer::getInstance()->DrawLinePerma(pos1, pos2, col);
	Renderer::getInstance()->DrawLinePerma(pos2, pos3, col);
	Renderer::getInstance()->DrawLinePerma(pos3, pos0, col);

	Renderer::getInstance()->DrawLinePerma(pos4, pos5, col);
	Renderer::getInstance()->DrawLinePerma(pos5, pos6, col);
	Renderer::getInstance()->DrawLinePerma(pos6, pos7, col);
	Renderer::getInstance()->DrawLinePerma(pos7, pos4, col);

	Renderer::getInstance()->DrawLinePerma(pos0, pos7, col);
	Renderer::getInstance()->DrawLinePerma(pos3, pos4, col);
	Renderer::getInstance()->DrawLinePerma(pos2, pos5, col);
	Renderer::getInstance()->DrawLinePerma(pos6, pos1, col);;
}

void Renderer::DrawDebug(RenderTarget& rt, const CCamera*cam)
{
	if (!mDebugLinesCount)
		return;

	//DebugGrid(50, 50, Vector4(.7f, .7f, .7f, 1));

	//ClearDepthBuffer();
	EnableDepthWriting(false);
	//SetViewport(mOculusRect);

	// Debug Line Draw
	md3dImmediateContext->OMSetRenderTargets(1, rt.GetRenderTargetViewHandle(), NULL);
	ShaderManager::Bind(Shader::DEBUG);
  InputLayouts::SetInputLayout(InputLayouts::InputLayout_Debug);

	//lines should already be in their world space
  ConstantBufferManager::BindCameraBuffer(cam->ProjView());

	LineMesh* debugMesh = AssetManager::getInstance()->GetAsset("DebugMesh").GetAssetPtr<LineMesh>();
	SetPrimitiveType(debugMesh->GetPrimitiveType());

	unsigned counter = 0;
	while (counter < mDebugLinesCount)
	{
		unsigned lineSize = 0;

		if (mDebugLinesCount - counter >= MAXLINES)
			lineSize = MAXLINES;
		else
			lineSize = mDebugLinesCount % MAXLINES;

		debugMesh->BeginMesh();

		if (debugMesh->GetLineCount() != lineSize)
			debugMesh->GetLineVertices().resize(lineSize);

		memcpy(debugMesh->GetLineVertices().data(), mDebugLines.data() + counter, sizeof(LineVertex) * lineSize);

		debugMesh->EndMesh(md3dDevice);

		debugMesh->BindMesh(md3dImmediateContext);
		debugMesh->DrawMesh(md3dImmediateContext);

		counter += lineSize;
	}

	EnableDepthWriting(true);
}

void Renderer::EnableDebugDraw(bool enable)
{
	mEnableDebugDraw = enable;
}

void Renderer::ToggleDebugDraw()
{
	EnableDebugDraw(!mEnableDebugDraw);
}

D3D_FEATURE_LEVEL Renderer::GetFeatureLevel()
{
	return this->featureLevel;
}

void Renderer::SetBlendState(CRenderer::BlendState blendState)
{
	if (blendState == currentBlendState)
		return;

	float mask[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	switch (blendState)
	{
	case CRenderer::ALPHA_BLEND:
		EnableDepthWriting(true);
		md3dImmediateContext->OMSetBlendState(mAlphaBlendEnabled, 0, 0xFFFFFF);
		currentBlendState = blendState;
		break;

  case CRenderer::ADDITIVE_BLEND:
		EnableDepthWriting(true);
		md3dImmediateContext->OMSetBlendState(mAdditiveBlending, mask, 0xffffff);
		currentBlendState = blendState;
		break;

  case CRenderer::DEFERRED_BLENDPASS_1:
		md3dImmediateContext->OMSetBlendState(0, 0, 0);
		md3dImmediateContext->OMSetDepthStencilState(mDeferredStecilMask1, 0);
		currentBlendState = blendState;
		break;

  case CRenderer::DEFERRED_BLENDPASS_2:
		md3dImmediateContext->OMSetBlendState(mAdditiveBlending, blendFactor, 0xFFFFFFFF);
		md3dImmediateContext->OMSetDepthStencilState(mDeferredStecilMask2, 0);
		currentBlendState = blendState;
		break;

  case CRenderer::COLOR_BLEND:
		md3dImmediateContext->OMSetBlendState(mColorBlendEnabled, blendFactor, 0xffffff);
		currentBlendState = blendState;
		break;

  case CRenderer::NO_BLEND_DEPTH_LESS_EQUAL:
    md3dImmediateContext->OMSetDepthStencilState(mDepthWriting_LESSEQUAL, 0);
    md3dImmediateContext->OMSetBlendState(NULL, mask, 0xffffffff);
    currentBlendState = blendState;
    break;

  case CRenderer::NO_BLEND_DEPTH_LESS:
	default:
		EnableDepthWriting(true);
		md3dImmediateContext->OMSetBlendState(NULL, mask, 0xffffffff);
		currentBlendState = blendState;
		break;
	};
}

void Renderer::SetRasterState(RasterState RState)
{
	switch (RState)
	{
	case Renderer::RENDER_FRONT:
		md3dImmediateContext->RSSetState(mRenderFront);
		break;
	case Renderer::RENDER_BACK:
		md3dImmediateContext->RSSetState(mRenderBack);
		break;
	}
}

void Renderer::CopyCamRenderTarget(RenderTarget& dest, CCamera* cam)
{
	Mesh* quad = AssetManager::getInstance()->GetAsset("Quad.mesh").GetAssetPtr<Mesh>();

	md3dImmediateContext->OMSetRenderTargets(1, dest.GetRenderTargetViewHandle(), NULL);
	md3dImmediateContext->PSSetShaderResources(0, 1, cam->GetRenderTarget()->GetShaderResourceHandle());
	ShaderManager::Bind(Shader::COPY_RESIZE);
	ConstantBufferManager::BindCameraBuffer(cam->PostProcessMatrix());
	quad->BindMesh(md3dImmediateContext);
	quad->DrawMesh(md3dImmediateContext);
}

void Renderer::SetViewport(const Rect& vp)
{
	D3D11_VIEWPORT D3DViewport;
	D3DViewport.TopLeftX = (float)vp.x;
	D3DViewport.TopLeftY = (float)vp.y;
	D3DViewport.Width = (float)vp.width;
	D3DViewport.Height = (float)vp.height;
	D3DViewport.MinDepth = 0;
	D3DViewport.MaxDepth = 1;
	md3dImmediateContext->RSSetViewports(1, &D3DViewport);
}

void Renderer::DebugGrid(const float& width, const float& height, const Color& color, const float& tilesize)
{
	Vector3 offset(-width * 0.5f, 0.0f, -height * 0.5f);

	for (float i = 0; i <= width; i += tilesize)
	{
		DrawLinePerma(Vector3(i, 0.0f, 0.0f) + offset, Vector3(i, 0.0f, height) + offset, color);
		DrawLinePerma(Vector3(0.0f, 0.0f, i) + offset, Vector3(width, 0.0f, i) + offset, color);
	}
}

void Renderer::ClearRenderQueues()
{
  mMapDeferredQueues.clear();
  mMapForwardQueues.clear();
  mMapDiffuseQueues.clear();
  mMapFontQueues.clear();
  mMapLightQueues.clear();
}

IDXGISwapChain* Renderer::GetOculusSwapChain()
{
  return mOculusSwapChain;
}

bool Renderer::IsTextureBoundToSlot(ID3D11ShaderResourceView** resourceViewPointer, unsigned slot)
{
	if (slot >= mBoundTextures.size())
		return true;

	return (mBoundTextures[slot] == resourceViewPointer);
}

void Renderer::BindTextureNode(ID3D11ShaderResourceView** resourceViewPointer, unsigned slot)
{
	if (IsTextureBoundToSlot(resourceViewPointer, slot))
		return;

	if (!resourceViewPointer)
	{
		ID3D11ShaderResourceView* nullShaderPtr = 0;
		GetD3D11DeviceContext()->PSSetShaderResources(slot, 1, &nullShaderPtr);
	}
	else
	{
		GetD3D11DeviceContext()->PSSetShaderResources(slot, 1, resourceViewPointer);
	}

	mBoundTextures[slot] = resourceViewPointer;
}

RenderTarget* Renderer::GetInputFXBuffer()
{
	return mInputFXBuffer;
}

RenderTarget* Renderer::GetOutputFXBuffer()
{
	return mOutputFXBuffer;
}

void Renderer::SwapTempFXBuffers()
{
	if (mInputFXBuffer == &mColor0)
	{
		mInputFXBuffer = &mColor1;
		mOutputFXBuffer = &mColor0;
	}
	else
	{
		mInputFXBuffer = &mColor0;
		mOutputFXBuffer = &mColor1;
	}
}

meta_define(Renderer);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change