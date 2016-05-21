/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Material.h"
#include "TextureNode.h"
#include "ColorNode.h"
#include "UVTransformNode.h"

void Material::SetTextureOffset(const Vector2& uvOffset, int textureIndex /*= 0*/)
{
  ColorNode* node = GetAttribute<ColorNode>(textureIndex);
	if (node)
		node->SetTextureOffset(uvOffset);
}

void Material::SetTextureScale(const Vector2& uvScale, int textureIndex /*= 0*/)
{
  ColorNode* node = GetAttribute<ColorNode>(textureIndex);
	if (node)
		node->SetTextureScale(uvScale);
}

unsigned int Material::GetTextureWidth( unsigned int textureIndex /*= 0*/ )
{
	TextureNode* node = GetAttribute<TextureNode>(textureIndex);
	if (node)
		return (unsigned)node->GetTextureWidth();
	return 0;
}

unsigned int Material::GetTextureHeight( unsigned int textureIndex /*= 0*/ )
{
	TextureNode* node = GetAttribute<TextureNode>(textureIndex);
	if (node)
		return (unsigned)node->GetTextureHeight();
	return 0;
}

float Material::GetColorAlpha()
{
	ColorNode* node = GetAttribute<ColorNode>();
	if (node)
		return (float)node->color.a;
	return 0;
}

void Material::SetColorAlpha(float alpha)
{
	ColorNode* node = GetAttribute<ColorNode>();
	if (node)
		node->color.a = alpha;
}

Material::~Material()
{
	Clear();
}

void Material::BindAttributes(Shader* overrideShader)
{
	if(!mMaterialData.GetAttributeCount())
		RebuildMaterial();

	mMaterialData.BindAttributes(overrideShader);
}

Material* Material::Clone()
{
	return new Material(*this);
}

void Material::Clear()
{
	mMaterialData.ClearAttributes();
}

RenderTarget* Material::GetTexture()
{
	TextureNode* node = GetAttribute<TextureNode>();
	if (node)
		node->GetTexture().GetAssetPtr<RenderTarget>();
	return 0;
}

Material::Material(const Material& v) : Asset(v)
{
	*this = v;
}

// Add Default Shader
Material::Material()
{
	RebuildMaterial();
}

Material& Material::operator=( const Material& rhs )
{
	//deep copy attributes
	Clear();
	mMaterialData = rhs.mMaterialData;

	return *this;
}

Shader* Material::GetShader()
{
	return mMaterialData.GetShader();
}

void Material::SetTexture( const std::string& textureName )
{
	GetAttribute<TextureNode>()->SetTexture(textureName);
}

void Material::SetTexture(const AssetHandle& assetHandle)
{
  GetAttribute<TextureNode>()->SetTexture(assetHandle);
}


void Material::SetShader( const char* shaderName )
{
	ShaderNode* shaderNode = reinterpret_cast<ShaderNode*>(GetAttribute<ShaderNode>());
	shaderNode->SetShader(shaderName);
	RebuildMaterial();
}

void Material::RebuildMaterial()
{
	Shader* currentShader = mMaterialData.GetShader();

	ShaderPrototypeVector prototypeVector = currentShader->GetShaderPrototype();
	std::map<unsigned, std::vector<MatAttribNode*>> newAttributes;

	for(ShaderPrototypeIter iter = prototypeVector.begin(); iter != prototypeVector.end(); ++iter)
	{
		unsigned index = 0;
		if (newAttributes.find(iter->NodeType) != newAttributes.end())
      index = newAttributes[iter->NodeType].size();
    MatAttribNode* attribute = GetAttribute(iter->NodeType, index);

		if(attribute)
		{
			attribute = attribute->CopyNode();
		}
		else
		{
			switch (iter->NodeType)
			{
				case MatAttribNode_Type::TEXTURE:
					attribute = new TextureNode((TextureNode::TextureRegisterType)index);
				break;
				case MatAttribNode_Type::COLOR:
					attribute = new ColorNode();
				break;
				//case MatAttribNode_Type::UV:
				//	attribute = new UVTransformNode();
				//break;
				case MatAttribNode_Type::SHADER:
					attribute = new ShaderNode();
				break;
			};
		}

    newAttributes[iter->NodeType].push_back(attribute);
	}

	Clear();
	for (auto& vec : newAttributes)
		for (std::vector<MatAttribNode*>::iterator iter = vec.second.begin(); iter != vec.second.end(); ++iter)
			mMaterialData.mAttributeMetaLookup[(*iter)->mNodeType].push_back(*iter);
}

void Material::OnInit()
{
	RebuildMaterial();
}

MatAttribNode* Material::GetAttribute(unsigned type_id, unsigned index)
{
	return mMaterialData.GetAttribute(type_id, index);
}

void Material::SetColor(float r, float g, float b, float a /*= 1.0f*/)
{
	ColorNode* node = GetAttribute<ColorNode>();
	if (node)
		node->color = Color(r, g, b, a);
}

void Material::SetColor(const Color& col)
{
	ColorNode* node = GetAttribute<ColorNode>();
	if (node)
		node->color = col;
}

Color Material::GetColor()
{
	ColorNode* node = GetAttribute<ColorNode>();
	if (node)
		return node->color;
	return 0;
}
