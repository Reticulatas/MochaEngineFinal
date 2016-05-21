#pragma once

#include "Shader.h"

class ShaderManager
{
public:

  static void Init();
  static void Bind(Shader::ShaderType shaderType);
  static void Clear();
  static Shader* mShaderContainer[Shader::TOTAL];
  static Shader::ShaderType mShaderCurrentlyBound;
  static void EnableSubShaders(Shader::ShaderType shader, unsigned char subshaderMask = 255);
  static void DisableSubShaders(Shader::ShaderType shader, unsigned char subshaderMask = 255);
};