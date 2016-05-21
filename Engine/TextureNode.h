/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once

#include "MatAttribNode.h"
#ifndef MOCHAEDITOR
#include "AssetManager.h"
#endif

struct ID3D10EffectShaderResourceVariable;

class TextureNode : public MatAttribNode
{
	friend class MaterialAttributeNodeComponent;

public:
  static MatAttribNode_Type::TypeEnum static_node_type;
	AssetHandle mTexture;

	enum TextureRegisterType
	{
		TEXTURE_DIFFUSE,
		TEXTURE_NORMAL,
		TEXTURE_SPECULAR,
		TEXTURE_ROUGHNESS,
		TEXTURE_METALLIC
	};
	TextureRegisterType mTextureRegister;

	virtual void BindAttribute(Shader* shader);
	//virtual void SetAttributeName(const std::string& name);
	void SetTexture(AssetHandle hnd);
	void SetTexture(const std::string& textureName);
	float GetTextureWidth();
	float GetTextureHeight();
	AssetHandle& GetTexture();
	void SetTextureRegister(TextureRegisterType type);
	TextureNode();
	TextureNode(TextureRegisterType textureRegister);
	virtual ~TextureNode();
	virtual MatAttribNode* CopyNode();

	___SERIALIZE_BEGIN___
		___DEFBASE(MatAttribNode)
		___DEFSER(mTexture, 1)
		___DEFSER(mTextureRegister, 1)
		SetTexture(mTexture);
	___SERIALIZE_END___

		metadef(TextureNode)
		m_addbase(MatAttribNode);
	endmetadef;
};
___SERIALIZE_CLASS(TextureNode, 1);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change