/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */

#pragma once

#include "AssetManager.h"
#include "SubShader.h"

struct ShaderNodePrototype
{
  MatAttribNode_Type::TypeEnum NodeType;
  //std::string NodeName;
  //unsigned typeID;		//typeid from the meta system

  bool operator== (const ShaderNodePrototype& rhs) const;
};

typedef std::vector<ShaderNodePrototype> ShaderPrototypeVector;
typedef ShaderPrototypeVector::iterator ShaderPrototypeIter;

class Shader : public Asset
{
public:

  enum ShaderType
  {
    DEFERRED_GEOMETRY,
    DEFERRED_LIGHT,
    ANIMATION,
    DEBUG,
    FORWARD_RENDER,
    DIFFUSE_RENDER,
    POSTPROCESS,
    FX_BLOOM,
    COPY_RESIZE,
	  FX_GRAINY,
    FX_PIXELATE,
    FX_MOTIONBLUR,
    FX_CENSORX,
    FX_CENSORY,
    FX_BLURX,
    FX_BLURY,
	  FX_CHROMATIC,
    FX_ARTIFACTS,
    PARTICLE_SHADER,
	  TONE_MAPPING,
    DISTORTION_RENDER,
    FX_TRIANGLE,
    FXAA,
	FX_LENSFLARE,
    TOTAL,
    UNASSIGNED
  };

  Shader(); 
  SubShader<ID3D11VertexShader>* GetVertexShader();
  SubShader<ID3D11PixelShader>* GetPixelShader();
  SubShader<ID3D11ComputeShader>* GetComputeShader();
  SubShader<ID3D11GeometryShader>* GetGeometryShader();

  void AddSubShader(const std::string& subshaderFile);

  void BuildDefaultShaderPrototype();
  void AddShaderNodePrototype(ShaderNodePrototype& shaderPrototype);
  ShaderPrototypeVector& GetShaderPrototype();			/* Returns the vector of Shader node prototypes */

  ShaderType GetShaderType() const;
  void SetShaderType(const ShaderType& shadertype);

  /// Sub Shader Masking
  void EnableSubShaders(unsigned char enabledMask = 255);
  void DisableSubShaders(unsigned char disableMask = 255);

private:
  template <typename T>
  SubShader<T>* GetShader();

  unsigned char mEnabledSubShadersMask;
  ShaderPrototypeVector mShaderNodePrototype;
  std::map<unsigned, Asset*> mSubShaderList;
  ShaderType mShaderType;
};

template <typename T>
SubShader<T>* Shader::GetShader()
{
	auto iter = mSubShaderList.find(SubShader<T>::getmeta().type_id);
	if (iter != mSubShaderList.end())
		return dynamic_cast<SubShader<T>*>(iter->second);

	return 0;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change