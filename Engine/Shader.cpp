#include "stdafx.h"
#include "Shader.h"
#include "SubShader.h"
#include "TextureNode.h"
#include "ColorNode.h"
#include "UVTransformNode.h"
/*****************************************************************
*
*			Shader Node Prototype
*
******************************************************************/

bool ShaderNodePrototype::operator==(const ShaderNodePrototype& rhs) const
{
  if (this->NodeType != rhs.NodeType)
    return false;

  //return (this->typeID == rhs.typeID);
  return true;
}

ShaderPrototypeVector& Shader::GetShaderPrototype()
{
  return mShaderNodePrototype;
}

void Shader::AddShaderNodePrototype(ShaderNodePrototype& shaderPrototype)
{
  mShaderNodePrototype.push_back(shaderPrototype);
}

void Shader::BuildDefaultShaderPrototype()
{
  ShaderNodePrototype shaderNode;

  shaderNode.NodeType = MatAttribNode_Type::SHADER;
  //shaderNode.typeID = ShaderNode::getmeta().type_id;
  AddShaderNodePrototype(shaderNode);

  // Diffuse
  shaderNode.NodeType = MatAttribNode_Type::TEXTURE;
  //shaderNode.typeID = TextureNode::getmeta().type_id;
  AddShaderNodePrototype(shaderNode);

  // Normal
  shaderNode.NodeType = MatAttribNode_Type::TEXTURE;
  //shaderNode.typeID = TextureNode::getmeta().type_id;
  AddShaderNodePrototype(shaderNode);

  // Specular
  shaderNode.NodeType = MatAttribNode_Type::TEXTURE;
  //shaderNode.typeID = TextureNode::getmeta().type_id;
  AddShaderNodePrototype(shaderNode);

  // Roughness
  shaderNode.NodeType = MatAttribNode_Type::TEXTURE;
  //shaderNode.typeID = TextureNode::getmeta().type_id;
  AddShaderNodePrototype(shaderNode);

  // Metallic
  shaderNode.NodeType = MatAttribNode_Type::TEXTURE;
  //shaderNode.typeID = TextureNode::getmeta().type_id;
  AddShaderNodePrototype(shaderNode);

  shaderNode.NodeType = MatAttribNode_Type::COLOR;
  //shaderNode.typeID = ColorNode::getmeta().type_id;
  AddShaderNodePrototype(shaderNode);

  //shaderNode.NodeType = MatAttribNode_Type::UV;
  //shaderNode.typeID = UVTransformNode::getmeta().type_id;
  //AddShaderNodePrototype(shaderNode);
}

Shader::Shader() : Asset()
{
  BuildDefaultShaderPrototype();
  mShaderType = UNASSIGNED;
  EnableSubShaders();
}

void Shader::AddSubShader(const std::string& subshaderFile)
{
  Asset* newSubShader = AssetManager::getInstance()->GetAsset(subshaderFile).GetAssetPtr<Asset>();  
  mSubShaderList[newSubShader->mytopid()] = newSubShader;
}

SubShader<ID3D11VertexShader>* Shader::GetVertexShader()
{
	if (mEnabledSubShadersMask & VERTEX_SHADER)
		return GetShader<ID3D11VertexShader>();
	return 0;
}

SubShader<ID3D11PixelShader>* Shader::GetPixelShader()
{
	if (mEnabledSubShadersMask & PIXEL_SHADER)
		return GetShader<ID3D11PixelShader>();
	return 0;
}
SubShader<ID3D11ComputeShader>* Shader::GetComputeShader()
{
	if (mEnabledSubShadersMask & COMPUTE_SHADER)
		return GetShader<ID3D11ComputeShader>();
	return 0;
}
SubShader<ID3D11GeometryShader>* Shader::GetGeometryShader()
{
	if (mEnabledSubShadersMask & GEOMETRY_SHADER)
		return GetShader<ID3D11GeometryShader>();
	return 0;
}

Shader::ShaderType Shader::GetShaderType() const
{
  return mShaderType;
}

void Shader::SetShaderType(const ShaderType& shadertype)
{
  mShaderType = shadertype;
}

void Shader::EnableSubShaders(unsigned char enabledMask/* = 255*/)
{
	mEnabledSubShadersMask |= enabledMask;
}
void Shader::DisableSubShaders(unsigned char disableMask /* = 255*/)
{
	mEnabledSubShadersMask &= ~disableMask;
}


