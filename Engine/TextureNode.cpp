/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "TextureNode.h"
#include "Shader.h"
#include "GraphicsInclude.h"

TextureNode::TextureNode():MatAttribNode(MatAttribNode_Type::TEXTURE)
{
  mTextureRegister = TEXTURE_DIFFUSE;
  //SetTexture("missingTexture.png");
}

TextureNode::TextureNode(TextureRegisterType textureRegister) : MatAttribNode(MatAttribNode_Type::TEXTURE)
{
  mTextureRegister = textureRegister;
}

void TextureNode::BindAttribute( Shader* shader )
{
	if (mTexture.Valid())
	{
		Renderer::getInstance()->BindTextureNode(mTexture.GetAssetPtr<RenderTarget>()->GetShaderResourceHandle(), mTextureRegister);
		//Renderer::getInstance()->GetD3D11DeviceContext()->PSSetShaderResources(mTextureRegister, 1, mTexture.GetAssetPtr<RenderTarget>()->GetShaderResourceHandle());
	}
	else
	{
		Renderer::getInstance()->BindTextureNode(0, mTextureRegister);
	}
    
}

TextureNode::~TextureNode()
{

}

void TextureNode::SetTexture( AssetHandle hnd )
{
  mTexture = hnd;
}

void TextureNode::SetTexture( const std::string& textureName )
{
  mTexture = AssetManager::getInstance()->GetAsset(textureName);
}

MatAttribNode* TextureNode::CopyNode()
{
  TextureNode* node = new TextureNode;
  if (this->mTexture.Valid() && !this->mTexture.GetRawAssetPtr()->IsVirtual())
	  node->SetTexture((this->mTexture.GetName()));
  node->mNodeType = this->mNodeType;
  node->mTextureRegister = mTextureRegister;
  return reinterpret_cast<MatAttribNode*>(node);
}

void TextureNode::SetTextureRegister(TextureRegisterType type)
{
  mTextureRegister = type;
}

float TextureNode::GetTextureWidth()
{
	if (mTexture.Valid())
		return (float)mTexture.GetAssetPtr<RenderTarget>()->GetRenderTargetWidth();
	return 0;
}

float TextureNode::GetTextureHeight()
{
	if (mTexture.Valid())
		return (float)mTexture.GetAssetPtr<RenderTarget>()->GetRenderTargetHeight();
	return 0;
}

AssetHandle& TextureNode::GetTexture()
{
	return mTexture;
}

MatAttribNode_Type::TypeEnum TextureNode::static_node_type = MatAttribNode_Type::TEXTURE;

meta_define(TextureNode);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
