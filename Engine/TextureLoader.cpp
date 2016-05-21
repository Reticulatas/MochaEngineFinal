#include "stdafx.h"
#include "TextureLoader.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

RenderTarget* TextureLoader::LoadTexture(const std::string& filePath, unsigned bindFlags)
{
  HRESULT hr;
  RenderTarget* pTexture = new RenderTarget();

  pTexture->SetAutoUnload(false);
  pTexture->AddUseCounter();

  ID3D11Device* device = Renderer::getInstance()->GetD3D11Device();

  std::wstring file(filePath.begin(), filePath.end());

  // Create the texture resource and use a temp srv - we want to create srv ourselves
  ID3D11ShaderResourceView* tempSRV = 0;
  hr = DirectX::CreateWICTextureFromFileEx(
       device,                                          // DirectX Device
       file.c_str(),                                    // Filename
       0,                                               // Max size - 0 means let DirectX handle the limits
       D3D11_USAGE_DEFAULT,                             // Usage flags
       bindFlags,                                       // Bind flags
       0,                                               // CPU usage flags
       0,                                               // Misc flags
       false,                                           // SRGB
       (ID3D11Resource**)pTexture->GetTextureHandle(),  // Resource Handle
       &tempSRV);                                       // SRV Handle

  if (FAILED(hr))
  {
    delete pTexture;
    std::string ex = "Failed to load " + filePath;
    throw std::exception(ex.c_str());
  }

  ReleaseCOM(tempSRV);

  // Create SRV or/and RT ourselves
  pTexture->CreateFromTexture();

  return pTexture;
}

RenderTarget* TextureLoader::LoadDDSTexture(const std::string& filePath)
{
  HRESULT hr;
  RenderTarget* pTexture = new RenderTarget();

  pTexture->SetAutoUnload(false);
  pTexture->AddUseCounter();

  ID3D11Device* device = Renderer::getInstance()->GetD3D11Device();

  std::wstring file(filePath.begin(), filePath.end());

  // Create the texture resource and use a temp srv - we want to create srv ourselves
  ID3D11ShaderResourceView* tempSRV = 0;
  hr = DirectX::CreateDDSTextureFromFileEx(
       device,                                          // DirectX Device
       file.c_str(),                                    // Filename
       0,                                               // Max size - 0 means let DirectX handle the limits
       D3D11_USAGE_DEFAULT,                             // Usage flags
       D3D11_BIND_SHADER_RESOURCE,                      // Bind flags
       0,                                               // CPU usage flags
       0,                                               // Misc flags
       false,                                           // SRGB
       (ID3D11Resource**)pTexture->GetTextureHandle(),  // Resource Handle
       &tempSRV);                                       // SRV Handle

  ReleaseCOM(tempSRV);

  if (FAILED(hr))
  {
    delete pTexture;
    std::string ex = "Failed to load " + filePath;
    throw std::exception(ex.c_str());
  }

  // Create SRV or/and RT ourselves
  pTexture->CreateFromCubeMapTexture();

  return pTexture;
}

RenderTarget* TextureLoader::LoadTextureFromMemory(const unsigned char* buffer, const unsigned& bufferSize, unsigned bindFlags)
{
  HRESULT hr;
  RenderTarget* pTexture = new RenderTarget();

  pTexture->SetAutoUnload(false);
  pTexture->AddUseCounter();

  ID3D11Device* device = Renderer::getInstance()->GetD3D11Device();

  // Create the texture resource and use a temp srv - we want to create srv ourselves
  ID3D11ShaderResourceView* tempSRV = 0;
  hr = DirectX::CreateWICTextureFromMemoryEx(
    device,
    buffer,
    bufferSize,
    0,
    D3D11_USAGE_DEFAULT,
    bindFlags,
    0,
    0,
    false,
    (ID3D11Resource**)pTexture->GetTextureHandle(),
    &tempSRV);

  ReleaseCOM(tempSRV);

  if (FAILED(hr))
  {
    delete pTexture;
    std::string ex = "Failed to load texture from memory\n";
    throw std::exception(ex.c_str());
  }

  // Create SRV or/and RT ourselves
  pTexture->CreateFromTexture();

  return pTexture;
}

TextureLoader* TextureLoader::getInstance()
{
  static TextureLoader* instance = 0;

  if (!instance)
    instance = new TextureLoader();

  return instance;
}