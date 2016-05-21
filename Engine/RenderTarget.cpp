/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Eduardo Chaves
-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "RenderTarget.h"
#include "Engine.h"
#include "Oculus.h"

// Constructor /  Destructor
RenderTarget::RenderTarget(bool isCubemap) : Asset(), mTargetTexture(0), mShaderResourceView(0), mScale(1), mIsCubeMap(isCubemap)
{
  ZeroMemory(&mTextureDesc, sizeof(mTextureDesc));
  ZeroMemory(&mRenderTargetViewDesc, sizeof(mRenderTargetViewDesc));
  ZeroMemory(&mShaderResourceViewDesc, sizeof(mShaderResourceViewDesc));
  ZeroMemory(&mDepthStencilDesc, sizeof(mDepthStencilDesc));

  if (isCubemap)
  {
    mRenderTargetViewArray.resize(6);
    mDepthStencilViewArray.resize(6);
  }
  else
  {
    mRenderTargetViewArray.resize(1);
    mDepthStencilViewArray.resize(1);
  }

  mActiveRenderTargetView = mActiveDepthStencilView = 0;

  SetType("Textures");
  mRTType = RenderTargetType (-1);
}

RenderTarget::~RenderTarget()
{
	ReleaseTarget();
}

// Returns true if succeeded
bool RenderTarget::InitRenderTarget(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag, float scaleSize)
{
  ReleaseTarget();
	ID3D11Device* d3dDevice = Renderer::getInstance()->GetD3D11Device();

  mScale = scaleSize;

  mTextureDesc.Width = unsigned(textureWidth * scaleSize);   //+ (unsigned)Engine::getInstance()->GetGameWnd()->GetScreenOffsetX();
  mTextureDesc.Height = unsigned(textureHeight * scaleSize); //+ (unsigned)Engine::getInstance()->GetGameWnd()->GetScreenOffsetY();
  mTextureDesc.MipLevels = 1;
  mTextureDesc.ArraySize = 1;

	mTextureDesc.Format = formatFlag;
	mTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	mTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
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

	HR(d3dDevice->CreateTexture2D(&mTextureDesc, 0, &mTargetTexture));

	mRenderTargetViewDesc.Format = mTextureDesc.Format;

	if (Renderer::getInstance()->Is4xMsaaEnabled())
	{
		mRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}
	// No MSAA
	else
	{
		mRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		mRenderTargetViewDesc.Texture2D.MipSlice = 0;
	}

	HR(d3dDevice->CreateRenderTargetView(mTargetTexture, &mRenderTargetViewDesc, &mRenderTargetViewArray[0]));


	mShaderResourceViewDesc.Format = mTextureDesc.Format;

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

bool RenderTarget::InitRenderTargetCube(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag, float scaleSize, int mipLevels)
{
	ReleaseTarget();
	ID3D11Device* d3dDevice = Renderer::getInstance()->GetD3D11Device();

	mScale = scaleSize;

	mTextureDesc.Width = NextPowerOfTwo(textureWidth);   // (unsigned)Engine::getInstance()->GetGameWnd()->GetScreenOffsetX();
	mTextureDesc.Height = mTextureDesc.Width; // (unsigned)Engine::getInstance()->GetGameWnd()->GetScreenOffsetY();
	mTextureDesc.MipLevels = mipLevels;
	// we need 6 textures for cubemaps
	mTextureDesc.ArraySize = 6;

	mTextureDesc.Format = formatFlag;
	mTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	mTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	mTextureDesc.CPUAccessFlags = 0;
	mTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

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

	HR(d3dDevice->CreateTexture2D(&mTextureDesc, 0, &mTargetTexture));

	mRenderTargetViewDesc.Format = mTextureDesc.Format;

	if (Renderer::getInstance()->Is4xMsaaEnabled())
	{
		mRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		mRenderTargetViewDesc.Texture2DMSArray.ArraySize = 1;
	}
	// No MSAA
	else
	{
	  mRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	  mRenderTargetViewDesc.Texture2DArray.MipSlice = 0;
	  mRenderTargetViewDesc.Texture2DArray.ArraySize = 1;
	}	

	// For cubemaps, create 6 render target views
  for (unsigned i = 0; i < mRenderTargetViewArray.size(); ++i)
	{
		mRenderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
		HR(d3dDevice->CreateRenderTargetView(mTargetTexture, &mRenderTargetViewDesc, &mRenderTargetViewArray[i]));
	}

	mShaderResourceViewDesc.Format = mTextureDesc.Format;

	mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	mShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
	mShaderResourceViewDesc.TextureCube.MipLevels = -1;


	HR(d3dDevice->CreateShaderResourceView(mTargetTexture, &mShaderResourceViewDesc, &mShaderResourceView));

	mRTType = RT_TEXTURECUBE;

	return true;
}

bool RenderTarget::InitRenderTargetDepth(unsigned textureWidth, unsigned textureHeight, DXGI_FORMAT formatFlag)
{
  ReleaseTarget();
  // No render target - Just Creating Depth stencil view and Depth SRV
  ID3D11Device* d3dDevice = Renderer::getInstance()->GetD3D11Device();

  mTextureDesc.Width = textureWidth;
  mTextureDesc.Height = textureHeight;
  mTextureDesc.MipLevels = 1;
  mTextureDesc.ArraySize = (mIsCubeMap) ? 6 : 1;
  mTextureDesc.Format = formatFlag;
  mTextureDesc.Usage = D3D11_USAGE_DEFAULT;
  mTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
  mTextureDesc.CPUAccessFlags = 0;
  mTextureDesc.MiscFlags = (mIsCubeMap) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

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

  HR(d3dDevice->CreateTexture2D(&mTextureDesc, 0, &mTargetTexture));

  ZeroMemory(&mDepthStencilDesc, sizeof(mDepthStencilDesc));  // Must zero out the rest

  mDepthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;

  if (Renderer::getInstance()->Is4xMsaaEnabled())
  {
    mDepthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
  }
  else
  {
    if (!mIsCubeMap)
    {
      mDepthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      mDepthStencilDesc.Texture2D.MipSlice = 0;
    }
    else
    {
      mDepthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
      mDepthStencilDesc.Texture2DArray.MipSlice = 0;
      mDepthStencilDesc.Texture2DArray.ArraySize = 1;
    }
  }  

  if (!mIsCubeMap)
  {
    HR(d3dDevice->CreateDepthStencilView(mTargetTexture, &mDepthStencilDesc, &mDepthStencilViewArray[0]));
  }
  else
  {
    for (unsigned i = 0; i < mDepthStencilViewArray.size(); ++i)
    {
      mDepthStencilDesc.Texture2DArray.FirstArraySlice = i;
      HR(d3dDevice->CreateDepthStencilView(mTargetTexture, &mDepthStencilDesc, &mDepthStencilViewArray[i]));
    }
  }  

  mShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;

  if (Renderer::getInstance()->Is4xMsaaEnabled())
  {
    mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
  }
  //No MSAA
  else
  {
    mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    mShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    mShaderResourceViewDesc.Texture2D.MipLevels = 1;

    if (mIsCubeMap)
    {
      mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
      mShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
      mShaderResourceViewDesc.TextureCube.MipLevels = -1;
    }    
  }

  HR(d3dDevice->CreateShaderResourceView(mTargetTexture, &mShaderResourceViewDesc, &mShaderResourceView));

  mRTType = (mIsCubeMap) ? RT_TEXTURECUBE : RT_DEPTHSTENCIL;

  return true;
}

// Returns true if succeeded
bool RenderTarget::ReleaseTarget(void)
{
	for (unsigned i = 0; i < mRenderTargetViewArray.size(); ++i)
	{
		ReleaseCOM(mRenderTargetViewArray[i]);
		mRenderTargetViewArray[i] = 0;
	}
		
  for (unsigned i = 0; i < mDepthStencilViewArray.size(); ++i)
  {
    ReleaseCOM(mDepthStencilViewArray[i]);
    mDepthStencilViewArray[i] = 0;
  }

	ReleaseCOM(mTargetTexture);
	ReleaseCOM(mShaderResourceView);

	
	mTargetTexture = 0;
	mShaderResourceView = 0;

	ZeroMemory(&mTextureDesc, sizeof(mTextureDesc));
	ZeroMemory(&mRenderTargetViewDesc, sizeof(mRenderTargetViewDesc));
	ZeroMemory(&mShaderResourceViewDesc, sizeof(mShaderResourceViewDesc));
	ZeroMemory(&mDepthStencilDesc, sizeof(mDepthStencilDesc));

	return true;
}

// Getters and Setters
const D3D11_TEXTURE2D_DESC&	RenderTarget::GetTextureDesc(void)
{
	return this->mTextureDesc;
}

const D3D11_RENDER_TARGET_VIEW_DESC& RenderTarget::GetRenderTargetViewDesc(void)
{
	return this->mRenderTargetViewDesc;
}

const D3D11_SHADER_RESOURCE_VIEW_DESC&	RenderTarget::GetShaderResourceViewDesc(void)
{
	return this->mShaderResourceViewDesc;
}

ID3D11RenderTargetView* RenderTarget::GetRenderTargetView(void)
{
	return this->mRenderTargetViewArray[mActiveRenderTargetView];
}

ID3D11RenderTargetView** RenderTarget::GetRenderTargetViewHandle(void)
{
	return &mRenderTargetViewArray[mActiveRenderTargetView];
}

ID3D11RenderTargetView* RenderTarget::GetRenderTargetView(unsigned index)
{
	return this->mRenderTargetViewArray[index];
}

ID3D11ShaderResourceView* RenderTarget::GetShaderResourceView(void)
{
	return mShaderResourceView;
}

ID3D11ShaderResourceView** RenderTarget::GetShaderResourceHandle(void)
{
  return &mShaderResourceView;
}

ID3D11DepthStencilView* RenderTarget::GetDepthStencilView()
{
  return mDepthStencilViewArray[mActiveDepthStencilView];
}

float RenderTarget::GetRenderTargetWidth() const
{
  return (float)mTextureDesc.Width;
}        

float RenderTarget::GetRenderTargetHeight() const
{
  return (float)mTextureDesc.Height;
}

RenderTarget::RenderTargetType RenderTarget::GetRenderTargetType(void)
{
	return mRTType;
}

ID3D11Texture2D** RenderTarget::GetTextureHandle()
{
  return &mTargetTexture;
}

ID3D11Texture2D* RenderTarget::GetTexture()
{
  return mTargetTexture;
}

const D3D11_DEPTH_STENCIL_VIEW_DESC& RenderTarget::GetDepthStencilViewDesc(void)
{
  return mDepthStencilDesc;
}

void RenderTarget::SetTextureDesc(const D3D11_TEXTURE2D_DESC& texDesc)
{
  mTextureDesc = texDesc;
}

void RenderTarget::SetRenderTargetViewDesc(const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc)
{
  mRenderTargetViewDesc = rtvDesc;
}

void RenderTarget::SetShaderResourceViewDesc(const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc)
{
  mShaderResourceViewDesc = srvDesc;
}

void RenderTarget::SetDepthStencilViewDesc(const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvDesc)
{
  mDepthStencilDesc = dsvDesc;
}

void RenderTarget::CreateFromTexture()
{
  ID3D11Device* d3dDevice = Renderer::getInstance()->GetD3D11Device();

  mTargetTexture->GetDesc(&mTextureDesc);

  mRenderTargetViewDesc.Format = mTextureDesc.Format;

  if (mTextureDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
  {
    if (Renderer::getInstance()->Is4xMsaaEnabled())
      mRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    // No MSAA
    else
    {
      mRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      mRenderTargetViewDesc.Texture2D.MipSlice = 0;
    }

    HR(d3dDevice->CreateRenderTargetView(mTargetTexture, &mRenderTargetViewDesc, &mRenderTargetViewArray[0]));
  }

  mShaderResourceViewDesc.Format = mTextureDesc.Format;

  if (Renderer::getInstance()->Is4xMsaaEnabled())
    mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

  // No MSAA
  else
  {
    mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    mShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    mShaderResourceViewDesc.Texture2D.MipLevels = 1;
  }

  HR(d3dDevice->CreateShaderResourceView(mTargetTexture, &mShaderResourceViewDesc, &mShaderResourceView));
}

void RenderTarget::SetActiveRenderTargetView(unsigned viewIndex)
{
	if (viewIndex < mRenderTargetViewArray.size())
		mActiveRenderTargetView = viewIndex;
}

void RenderTarget::SetActiveDepthStencilView(unsigned viewIndex)
{
  if (viewIndex < mDepthStencilViewArray.size())
    mActiveDepthStencilView = viewIndex;
}

void RenderTarget::CreateFromCubeMapTexture()
{
  ID3D11Device* d3dDevice = Renderer::getInstance()->GetD3D11Device();

  mTargetTexture->GetDesc(&mTextureDesc);

  mShaderResourceViewDesc.Format = mTextureDesc.Format;
  mShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  mShaderResourceViewDesc.TextureCube.MipLevels = mTextureDesc.MipLevels;
  mShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;

  HR(d3dDevice->CreateShaderResourceView(mTargetTexture, &mShaderResourceViewDesc, &mShaderResourceView));
}

void RenderTarget::Resize()
{
  DXGI_FORMAT format = mTextureDesc.Format;

  ReleaseTarget();

  unsigned textureWidth = Engine::getInstance()->GetVRWnd()->GetWindowWidth();
  unsigned textureHeight = Engine::getInstance()->GetVRWnd()->GetWindowHeight();

  if (Renderer::getInstance()->GetVR()->IsConnected())
  {
    textureWidth = Renderer::getInstance()->GetVR()->GetRecommendedWidth();
    textureHeight = Renderer::getInstance()->GetVR()->GetRecommendedHeight();
  }

  switch (mRTType)
  {
  case RT_TEXTURE2D:
	  InitRenderTarget(textureWidth, textureHeight, format, mScale);
	  break;
  case RT_DEPTHSTENCIL:
	  InitRenderTargetDepth(textureWidth, textureHeight, format);
	  break;
  case RT_TEXTURECUBE:
	  InitRenderTargetCube(textureWidth, textureHeight, format, mScale, 6);
	  break;
  }
}

//void RenderTarget::CopyFromRenderTarget(RenderTarget& SrcRT)
//{
//  ID3D11DeviceContext* context = Renderer::getInstance()->GetD3D11DeviceContext();
//
//  if (mTextureDesc.Width == SrcRT.mTextureDesc.Width &&
//    mTextureDesc.Height == SrcRT.mTextureDesc.Height &&
//    mTextureDesc.Format == SrcRT.mTextureDesc.Format)
//  {
//    context->CopyResource(mTargetTexture, SrcRT.mTargetTexture);
//    //context->CopyResource(mShaderResourceView, SrcRT.mShaderResourceView);
//    //context->CopyResource(mRenderTargetView, SrcRT.mRenderTargetView);
//  }
//}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change