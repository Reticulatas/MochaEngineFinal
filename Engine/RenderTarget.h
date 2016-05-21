/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Eduardo Chaves
-* See Footer for Revisions                                      */

#pragma once

#include <vector>
#include "GraphicsInclude.h"
#include "Asset.h"

class RenderTarget : public Asset
{
public:
	// Cubemap Texture Index
	enum CubeTextureIndex
	{
		CUBE_RIGHT = 0,
		CUBE_LEFT,
		CUBE_UP,
		CUBE_DOWN,
		CUBE_FRONT,
		CUBE_BACK
	};

	enum RenderTargetType
	{
		RT_TEXTURE2D,
		RT_DEPTHSTENCIL,
		RT_TEXTURECUBE
	};

private:

	ID3D11Texture2D*						          mTargetTexture;
	std::vector<ID3D11RenderTargetView*>	mRenderTargetViewArray;
	ID3D11ShaderResourceView*				      mShaderResourceView;
  std::vector<ID3D11DepthStencilView*>  mDepthStencilViewArray;

	D3D11_TEXTURE2D_DESC			      mTextureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC	  mRenderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC	mShaderResourceViewDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC   mDepthStencilDesc;

	// If we want a scaled render target size
	float mScale;
  bool mIsCubeMap;

	// When callsing getRenderTargetView
	unsigned mActiveRenderTargetView;
  unsigned mActiveDepthStencilView;
	RenderTargetType mRTType;

public:

	// Constructor /  Destructor
	RenderTarget(bool isCubemap = false);
	~RenderTarget();

	// Returns true if succeeded
	bool InitRenderTarget(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag, float scaleSize = 1);
	bool InitRenderTargetCube(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag, float scaleSize = 1, int mipLevels = 1);
	bool InitRenderTargetDepth(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag);
  template <typename T>
  bool InitRenderTargetFromMemory(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag, T* data);
	// Returns true if succeeded
	bool ReleaseTarget(void);
	void Resize();

	// Getters and Setters
	const D3D11_TEXTURE2D_DESC&				      GetTextureDesc(void);
	const D3D11_RENDER_TARGET_VIEW_DESC&	  GetRenderTargetViewDesc(void);
	const D3D11_SHADER_RESOURCE_VIEW_DESC&	GetShaderResourceViewDesc(void);
  const D3D11_DEPTH_STENCIL_VIEW_DESC&	  GetDepthStencilViewDesc(void);

	void SetTextureDesc(const D3D11_TEXTURE2D_DESC& texDesc);
	void SetRenderTargetViewDesc(const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc);
	void SetShaderResourceViewDesc(const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
  void SetDepthStencilViewDesc(const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvDesc);

	ID3D11Texture2D*           GetTexture();
	ID3D11Texture2D**          GetTextureHandle();
	ID3D11RenderTargetView*    GetRenderTargetView(void);
	ID3D11RenderTargetView**   GetRenderTargetViewHandle(void);
	ID3D11ShaderResourceView*  GetShaderResourceView(void);
	ID3D11ShaderResourceView** GetShaderResourceHandle(void);
	ID3D11DepthStencilView*    GetDepthStencilView();
	ID3D11RenderTargetView*    GetRenderTargetView(unsigned index);

	float GetRenderTargetWidth() const;
	float GetRenderTargetHeight() const;
	RenderTargetType GetRenderTargetType(void);

	void CreateFromTexture();
	void CreateFromCubeMapTexture();
	void SetActiveRenderTargetView(unsigned viewIndex);
  void SetActiveDepthStencilView(unsigned viewIndex);
};

template <typename T>
bool RenderTarget::InitRenderTargetFromMemory(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag, T* data)
{
  ReleaseTarget();
  ID3D11Device* d3dDevice = Renderer::getInstance()->GetD3D11Device();

  mTextureDesc.Width = unsigned(textureWidth);   //+ (unsigned)Engine::getInstance()->GetGameWnd()->GetScreenOffsetX();
  mTextureDesc.Height = unsigned(textureHeight); //+ (unsigned)Engine::getInstance()->GetGameWnd()->GetScreenOffsetY();
  mTextureDesc.MipLevels = 1;
  mTextureDesc.ArraySize = 1;
  mTextureDesc.Format = formatFlag;
  mTextureDesc.Usage = D3D11_USAGE_DEFAULT;
  mTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  mTextureDesc.CPUAccessFlags = 0;
  mTextureDesc.MiscFlags = 0;

  if (Renderer::getInstance()->Is4xMsaaEnabled())
  {
    mTextureDesc.SampleDesc.Count = 4;
    mTextureDesc.SampleDesc.Quality = Renderer::getInstance()->Get4xMsaaQuality() - 1;
  }
  // No MSAA
  else
  {
    mTextureDesc.SampleDesc.Count = 1;
    mTextureDesc.SampleDesc.Quality = 0;
  }

  D3D11_SUBRESOURCE_DATA subresData;
  ZeroMemory(&subresData, sizeof(D3D11_SUBRESOURCE_DATA));
  subresData.pSysMem = data;
  subresData.SysMemPitch = textureWidth * 4 * sizeof(T); // num of elements * size of each element
  HR(d3dDevice->CreateTexture2D(&mTextureDesc, &subresData, &mTargetTexture));

  // SRV
  mShaderResourceViewDesc.Format = mTextureDesc.Format;
  //mShaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

  if (Renderer::getInstance()->Is4xMsaaEnabled())
  {
    mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
  }
  // No MSAA
  else
  {
    mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    mShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    mShaderResourceViewDesc.Texture2D.MipLevels = 1;
  }

  HR(d3dDevice->CreateShaderResourceView(mTargetTexture, &mShaderResourceViewDesc, &mShaderResourceView));

  mRTType = RT_TEXTURE2D;

  return true;
}
/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change