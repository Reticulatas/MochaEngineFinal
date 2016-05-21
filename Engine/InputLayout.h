#pragma once

#include "Shader.h"

class InputLayoutDesc
{
public:
  static const D3D11_INPUT_ELEMENT_DESC Debug[2];
  static const D3D11_INPUT_ELEMENT_DESC Vert[6];
  static const D3D11_INPUT_ELEMENT_DESC SkinnedVert[8];
};

class InputLayouts
{
public:

  enum InputLayoutType
  {
    InputLayout_Vert,
    InputLayout_SkinnedVert,
    InputLayout_Debug,
    InputLayout_Null
  };

  static void Init(ID3D11Device* device);
  static void DestroyAll();
  static void SetInputLayout(InputLayoutType input);

  static ID3D11InputLayout* Debug;
  static ID3D11InputLayout* Vert;
  static ID3D11InputLayout* SkinnedVert;

private:

  static InputLayoutType layoutType;
};