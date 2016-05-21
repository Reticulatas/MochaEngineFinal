/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "ShaderNode.h"
#include "Shader.h"
#ifndef MOCHAEDITOR
#include "GraphicsInclude.h"
#endif

ShaderNode::ShaderNode(): MatAttribNode(MatAttribNode_Type::SHADER)
{
  mShaderFileName = "MochaDeferredGeometry.ms";
}

ShaderNode::~ShaderNode()
{

}
void ShaderNode::SetShader( const char* shaderName)
{
  mShaderFileName = shaderName;
  mShader = AssetManager::getInstance()->GetAsset(mShaderFileName).GetAssetPtr<Shader>();
}

void ShaderNode::SetShader( const std::string& shaderName )
{
	SetShader( shaderName.c_str() );
}

MatAttribNode* ShaderNode::CopyNode()
{
	ShaderNode* node = new ShaderNode;
  node->SetShader(mShaderFileName);
	node->mNodeType = this->mNodeType;
	return reinterpret_cast<MatAttribNode*>(node);
}

Shader* ShaderNode::GetShader()
{
	return mShader;
}

void ShaderNode::BindAttribute( Shader* shader )
{
	if(!mShader)
		return;

  ShaderManager::Bind(shader->GetShaderType());
}

MatAttribNode_Type::TypeEnum ShaderNode::static_node_type = MatAttribNode_Type::SHADER;

meta_define(ShaderNode);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
