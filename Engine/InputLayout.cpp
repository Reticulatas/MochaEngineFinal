#include "stdafx.h"
#include "InputLayout.h"
#include "GraphicsInclude.h"
#include "ShaderManager.h"

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Debug[2] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Vert[6] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD_ONE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::SkinnedVert[8] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD_ONE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::Debug = 0;
ID3D11InputLayout* InputLayouts::Vert = 0;
ID3D11InputLayout* InputLayouts::SkinnedVert = 0;
InputLayouts::InputLayoutType InputLayouts::layoutType = InputLayouts::InputLayoutType (-1);

void InputLayouts::Init(ID3D11Device* device)
{
  ////////
  // Debug
  ////////
  UINT numElements = sizeof(InputLayoutDesc::Debug) / sizeof(D3D11_INPUT_ELEMENT_DESC);
  SIZE_T bufferSize = ShaderManager::mShaderContainer[Shader::DEBUG]->GetVertexShader()->GetShaderBuffer()->GetBufferSize();
  
  // Create the input layout
  HR(device->CreateInputLayout(InputLayoutDesc::Debug, numElements, ShaderManager::mShaderContainer[Shader::DEBUG]->GetVertexShader()->GetShaderBuffer()->GetBufferPointer(),
    bufferSize, &Debug));

  ////////
  // Vertex layout
  ////////
  numElements = sizeof(InputLayoutDesc::Vert) / sizeof(D3D11_INPUT_ELEMENT_DESC);
  bufferSize = ShaderManager::mShaderContainer[Shader::DEFERRED_GEOMETRY]->GetVertexShader()->GetShaderBuffer()->GetBufferSize();
  
  // Create the input layout
  HR(device->CreateInputLayout(InputLayoutDesc::Vert, numElements, ShaderManager::mShaderContainer[Shader::DEFERRED_GEOMETRY]->GetVertexShader()->GetShaderBuffer()->GetBufferPointer(),
    bufferSize, &Vert));

  ////////
  // Skinned Vertex layout
  ////////
  numElements = sizeof(InputLayoutDesc::SkinnedVert) / sizeof(D3D11_INPUT_ELEMENT_DESC);
  bufferSize = ShaderManager::mShaderContainer[Shader::ANIMATION]->GetVertexShader()->GetShaderBuffer()->GetBufferSize();
  
  // Create the input layout
  HR(device->CreateInputLayout(InputLayoutDesc::SkinnedVert, numElements, ShaderManager::mShaderContainer[Shader::ANIMATION]->GetVertexShader()->GetShaderBuffer()->GetBufferPointer(),
    bufferSize, &SkinnedVert));
}

void InputLayouts::DestroyAll()
{
  ReleaseCOM(Debug);
  ReleaseCOM(Vert);
  ReleaseCOM(SkinnedVert);
}

void InputLayouts::SetInputLayout(InputLayoutType input)
{
  if (layoutType == input)
    return;

  layoutType = input;
  ID3D11InputLayout* inputLayout = 0;

  switch (input)
  {
  case InputLayouts::InputLayout_Vert:
    inputLayout = Vert;
    break;
  case InputLayouts::InputLayout_SkinnedVert:
    inputLayout = SkinnedVert;
    break;
  case InputLayouts::InputLayout_Debug:
    inputLayout = Debug;
    break;
  case InputLayouts::InputLayout_Null:
      inputLayout = nullptr;
      break;
  }

  Renderer::getInstance()->GetD3D11DeviceContext()->IASetInputLayout(inputLayout);
}
