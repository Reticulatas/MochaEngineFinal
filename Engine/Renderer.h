/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Eduardo Chaves | Kenneth Tan
  -* See Footer for Revisions                                      */


#pragma once 

#include <queue>
#include "CCamera.h"
#include "CMeshRenderer.h"
#include "CSkinMeshRenderer.h"
#include "CLight.h"
#include "RenderTarget.h"
#include "MathIncludes.h"
#include "Skeleton.h"
#include "RTDebugDraw.h"
#include "LineMesh.h"
#include "ISubsystem.h"
#include "VR.h"
#include "Oculus.h"
#include "Rectangle.h"
#include "CReflectionProbe.h"
#include "CPostProcess.h"

/* Class that will handle all the rendering.
it is a singleton */
// Enable the resource view when you want to call the shader using the texture buffer.  'context' is a ID3D11DeviceContext*.  The 'bindPoint' is known (or obtained using shader reflection).  'X' is in {'V', 'G', 'P', 'C'}.
class Renderer : public ISubsystem 
{  
private:

  ////////////////////////////////////////////
  ///////// Private Member Variables /////////
  ////////////////////////////////////////////
  /*	Direct 3D Specifics */
  ID3D11Device* md3dDevice;						    // Direct3D Device Handle
  ID3D11DeviceContext* md3dImmediateContext;		// D3D Immediate Context. Not deferred
  IDXGISwapChain* mSwapChain;						// Information about the SwapChain for Double Buffering
  IDXGISwapChain* mOculusSwapChain;
  D3D_DRIVER_TYPE md3dDriverType;					// Type of the D3D Driver - Usually Hardware 
  D3D_FEATURE_LEVEL featureLevel;					// Saves the feature level in the machine
  
  // View ports
  Rect mScreenVP;
  Rect mOculusVP;
  Rect mCubeMapVP;
  Rect mHalfScreenVP;
  Rect mLetterBoxVP;

  // Renderer info						
  UINT m4xMsaaQuality;						// If Texture multisampling enabled, determines the quality
  bool mEnable4xMsaa;							// True if Multisampling is enabled
  PrimitiveType mPrimitiveType;		// Primitive type, default to Triangle list
  bool mVSyncOn;									// Are we using Vsync?	
  bool mEnableFXAA;

  // Render state descriptions
  ID3D11BlendState*			    mAlphaBlendEnabled;
  ID3D11BlendState*			    mAlphaBlendDisabled;
  ID3D11DepthStencilState*	mDepthWriting_LESSEQUAL;
  ID3D11DepthStencilState*	mDepthWritingEnabled;
  ID3D11DepthStencilState*	mDepthWritingDisabled;
  ID3D11BlendState*			    mAdditiveBlending;

  // Deferred Stencil Mask  
  ID3D11DepthStencilState*  mDeferredStecilMask1;
  ID3D11DepthStencilState*  mDeferredStecilMask2;
  ID3D11BlendState*         mColorBlendEnabled;  // Turn off color write

  // Sampler state description
  ID3D11SamplerState* mPointSampler;
  ID3D11SamplerState* mAnisotropicSampler;
  ID3D11SamplerState* mClampAnisotropicSampler;
  ID3D11SamplerState* mClampBilinearSampler;

  // Rasterizer state
  ID3D11RasterizerState* mRenderFront;
  ID3D11RasterizerState* mRenderBack;

  // Rasterizer States
  enum RasterState
  {
    RENDER_FRONT,
    RENDER_BACK
  };

  // Optimizations Keeping track of bound textures
  std::vector<void*> mBoundTextures;

  // Render List Related
  //std::priority_queue<CMeshRenderer*, std::vector<CMeshRenderer*>, CRenderer::sortCRender> mMeshRenderQueue;	// Queue of Components that will be rendered
  //std::priority_queue<CSkinMeshRenderer*, std::vector<CSkinMeshRenderer*>, CRenderer::sortCRender> mSkinnedMeshRenderQueue;
  //std::priority_queue<CSkinMeshRenderer*, std::vector<CSkinMeshRenderer*>, CRenderer::sortCRender> mForwardCensorQueue;
 
  std::map<CCamera*, std::vector<CRenderer*>> mMapDeferredQueues;  // Meshes and Skinned that will be pumped into deferred pipeline
  std::map<CCamera*, std::vector<CRenderer*>> mMapForwardQueues;   // Meshes that have transparency
  std::map<CCamera*, std::vector<CRenderer*>> mMapDiffuseQueues;   // Meshes with just diffuse - unaffected by lights
  std::map<CCamera*, std::vector<CLight*>>    mMapLightQueues;     // Vector of current used lights
  std::map<CCamera*, std::vector<CRenderer*>> mMapFontQueues;      // Font rendering queue

  const std::list<CCamera*>* mCams;			
  const std::list<CReflectionProbe*>* mProbes;

  // Render Target format
  DXGI_FORMAT mFrameBufferFormat;
  DXGI_FORMAT mDepthBufferFormat;

  // Deferred Rendering related
  std::vector<ID3D11RenderTargetView*> mGBuffers;

  // GBuffers - SO MANY!?!?
  RenderTarget	mNormalsBuffer;
  RenderTarget	mAlbedoBuffer;
 // RenderTarget	mSpecularBuffer;
  RenderTarget  mVelocityBuffer;
  RenderTarget  mLightAccumBuffer;
  
  RenderTarget mDepthStencilBuffer;  // For Depth reading

  // PP-FX Buffers
  RenderTarget mColor0;
  RenderTarget mColor1;
  RenderTarget mColorHalf0;
  RenderTarget mColorHalf1;

  RenderTarget* mInputFXBuffer;
  RenderTarget* mOutputFXBuffer;

  // Final RT to the screen
  RenderTarget mFinalRT;		// View Handle of the Render Target. Used to set a texture as the Render Target

  // Oculus VR
  VR* mVR;
  RenderTarget mVRFinalRT;

  ////////////////////////////////////////////
  ///////// Private Member Functions /////////
  ////////////////////////////////////////////

  // Initialize Direct3D - expects the window handler
  bool InitD3D();

  // Clear the fame buffer to an specific color
  void ClearFrameBuffer(ID3D11RenderTargetView* rtv, const Color& clearCol);

  // Clear ZBuffer
  void ClearDepthBuffer(ID3D11DepthStencilView* dsv);

  // Clear Render Targets and Shader Resource View -> Causing warning
  void ClearRenderTargets();
  void ClearShaderResourceView();

  // Present the Backbuffer to the screen
  void SwapBuffers();								            				          

  // Builds the render list based on what is visible from the a specific camera
  void FrustumCulling_DoPass( CCamera* mCam, BaseState* state);
  void FrustumCulling(CCamera* mCam);

  // Set Up all render states
  void CreateRenderStates();

  // Set Up all sampler states
  void CreateSamplerStates();

  // Creates and resizes all textures buffers for our render targets
  void ResizeRenderTargets();

  // Generate the PerCamera and PerObject constant buffers
  bool CreateConstantBuffers();

  void SetRasterState(RasterState RState);

  // Creation of multiple swap chains for our game window and oculus window
  void CreateSwapChain(IDXGISwapChain** swapChain, MochaWindow* hWnd);

  void ResizeSwapChain(IDXGISwapChain* swapChain, RenderTarget& rt, MochaWindow* hWnd);

  // Renders our scene -> twice for the oculus
  void RenderScene(CCamera*cam);

  // Draw one reflection probe
  void DrawProbe(CCamera* probe);

  // Render all Probes
  void RenderProbeList(void);

  CReflectionProbe* GetBestReflectionProbe(CCamera* cam);

  void DrawDeferredGeometry(CCamera* cam);	      // Geometry Stage

  void DrawDeferredLighting(CCamera* cam);  // Lighting Stage

  void DrawParticles(CCamera* cam);  // Lighting Stage

  void DrawPostProcessing(const CCamera* cam);    // FX Stage

  void DrawDiffuseRenderer(CCamera* cam);           // Meshes not affected by light - for Editor
                                                  
  void DrawForwardRenderer(CCamera* cam);           // Forward Render - for Alpha / Transparency

  void DrawFontRenderer(CCamera* cam);
  
  void Clear();

  void ClearRenderQueues();

  void SetViewport(const Rect& vp);

  bool IsTextureBoundToSlot(ID3D11ShaderResourceView** resourceViewPointer, unsigned slot);

  void EarlyZPass(CCamera* cam);

  template <typename T>
  void ProcessRenderQueue(std::vector<T*>& renderQueue, const CCamera* cam);

  ////////////////////////////////////////////
  ///////// Debug Graphics related	 /////////
  ////////////////////////////////////////////
  unsigned mDebugLinesCount;
  std::vector<LineVertex> mDebugLines;
  bool mEnableDebugDraw;
  void DrawDebug(RenderTarget& rt, const CCamera*cam);
  void DebugGrid(const float& width, const float& height, const Color& color, const float& tilesize = 1.f);

  ////////////////////////////////////////////
  /////////		Post Processing		 /////////
  ////////////////////////////////////////////

  void PostProcess(RenderTarget* src0, RenderTarget* src1, RenderTarget* dest, Shader::ShaderType shader, bool clearFramebuffer = true);

  void FinalImageProcessing(RenderTarget* src, RenderTarget* dest, Rect* vp = 0); // Tone Mapping and FXAA

public:

  Vector4 mGlobalAmbient;

  void Init(VR* vr = 0);                  // Initialize the renderer system 
  bool Tick();									// Updates the renderer. must be called every frame
  void OnResize();

  // Renderer construction
  Renderer();											
  ~Renderer();									             
  static Renderer* getInstance();					    
  
  // Mesh Functionality 

  // Getters and Setters //
  PrimitiveType GetPrimitiveType();
  void SetPrimitiveType(PrimitiveType type);
  ID3D11Device* GetD3D11Device();
  ID3D11DeviceContext* GetD3D11DeviceContext();
  IDXGISwapChain* GetOculusSwapChain();
  VR* GetVR() { return mVR; }
  
  //VSync
  void EnableVSync(bool enable);
  bool IsVSyncEnabled();
  bool Is4xMsaaEnabled() { return mEnable4xMsaa; }
  UINT Get4xMsaaQuality() { return m4xMsaaQuality; }

  //TurnAlphaBlendingOnAndOff
  void EnableAlphaBlending(bool enabled );
  //TurnDepthWriting on and off
  void EnableDepthWriting(bool enable);

  CRenderer::BlendState currentBlendState;
  void SetBlendState(CRenderer::BlendState blendState);

  void EnableDebugDraw(bool enable);
  void ToggleDebugDraw();
  bool IsDebugDrawEnabled() { return mEnableDebugDraw; }

  void CopyCamRenderTarget(RenderTarget& dest, CCamera* cam);

  void BindTextureNode(ID3D11ShaderResourceView** resourceViewPointer, unsigned slot);	// Only binds a texture if it is not currently bound on that slot

  inline DXGI_FORMAT GetFrameBufferFormat(void){ return mFrameBufferFormat; }
  inline DXGI_FORMAT GetDepthBufferFormat(void){ return mDepthBufferFormat; }

  ////////////////////////////////
  //// POST PROCESSING //////////
  /////////////////////////////////
  RenderTarget* GetInputFXBuffer();
  RenderTarget* GetOutputFXBuffer();
  void SwapTempFXBuffers();

  ///////////////// DEBUG FUNCTIONS ////////////////////////////

  void DrawLine(const Vector3& begin, const Vector3& end, const Color& col = COLOR::WHITE); 
  void DrawLinePerma(const Vector3& begin, const Vector3& end, const Color& col = COLOR::WHITE); // Always draw lines
  void DrawBoxPerma(const Vector3& pos, const Vector3& halflengths, const Color& col = COLOR::WHITE);
  D3D_FEATURE_LEVEL GetFeatureLevel();

  // Friendships !!
  friend class EditorSubsystem;
  friend class GSM;
  friend class OculusVR;
  friend class SteamVR;
  friend class ParticleSystem;
  friend class FrustumCullingTask;
  friend class CPostProcess;
  friend class CBloom;
  friend class CMotionBlur;
  friend class CCensorFX;
  friend class CFilmGrain;
  friend class CChromaticAberration;
  friend class CArtifactFX;

  public:
	  metadef(Renderer)
		  endmetadef;
};
 
template <typename T>
void Renderer::ProcessRenderQueue(std::vector<T*>& renderQueue, const CCamera* cam)
{
  for (auto& currentElement : renderQueue)
  {
    if (!currentElement->GetMesh())
      continue;

    SetPrimitiveType(currentElement->GetMesh()->GetPrimitiveType());

    currentElement->BindBuffers(cam);

    if (currentElement->GetMaterial())
      currentElement->GetMaterial()->BindAttributes();

    Mesh* elementMesh = currentElement->GetMesh();
    elementMesh->BindMesh(md3dImmediateContext);
    elementMesh->DrawMesh(md3dImmediateContext);

    // Unbind Texture Shader Resource Views
    //ClearShaderResourceView();
  }
  ClearShaderResourceView();
}
/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change