%{
#include "Material.h"
#include "MaterialImpl.h"
#include "TextureNode.h"
#include "ShaderNode.h"
#include "UVTransformNode.h"
#include "ColorNode.h"
%}

class Material
{
	Material();
	~Material();
public:
	virtual void SetTexture(const std::string& textureName);
	void SetTextureOffset(Vector2& uvOffset, int textureIndex);
	void SetTextureScale(Vector2& uvScale, int textureIndex);
	unsigned GetTextureWidth(unsigned textureIndex);
	unsigned GetTextureHeight(unsigned textureIndex);
	float GetColorAlpha();
	void SetColorAlpha(float alpha);
	void SetColor(const Color& col);
	void SetColor(float r, float g, float b, float a = 1.0f);
	Color GetColor();

	MaterialImpl mMaterialData;
};

//////////////////////////////////

namespace MatAttribNode_Type
{
	enum TypeEnum
	{
		NONE,
		COLOR,
		TEXTURE,
		UV,
		SHADER
	};
}

%csattributes TextureNode::mTextureRegister "[MochaDoNotShow]"
%csattributes MatAttribNode::mNodeType "[MochaDoNotShow]"

class MatAttribNode : public IMeta
{
	MatAttribNode();
	~MatAttribNode();
public:
	MatAttribNode_Type::TypeEnum mNodeType;

	MatAttribNode_Type::TypeEnum NodeType() const;
};

class TextureNode : public MatAttribNode
{
	TextureNode();
	~TextureNode();

public:
	enum TextureRegisterType
	{
		TEXTURE_DIFFUSE,
		TEXTURE_NORMAL,
		TEXTURE_SPECULAR
	};
	AssetHandle mTexture;
	TextureRegisterType mTextureRegister;

	void SetTexture(AssetHandle hnd);
	void SetTexture(const std::string& textureName);
	void SetTextureRegister(TextureRegisterType type);
	float GetTextureWidth();
	float GetTextureHeight();
	AssetHandle& GetTexture();
};

class ColorNode : public MatAttribNode
{
	ColorNode();
	~ColorNode();
public:
	Color color;
  Math::Vector2 mTextureOffset;
  Math::Vector2 mTextureScale;

	virtual void BindAttribute(Shader* shader);
	void SetColor(const Color& col);
};

class ShaderNode : public MatAttribNode
{
	ShaderNode();
	~ShaderNode();
public:

	void SetShader(const std::string& shaderName);

	Shader* GetShader();
};

class UVTransformNode : public MatAttribNode
{
	UVTransformNode();
	~UVTransformNode();
public:
	//Math::Vector2 mTextureOffset;
	//Math::Vector2 mTextureScale;

	void SetTextureOffset(const Math::Vector2& offset);
	void SetTextureScale(const Math::Vector2& scale);
	void SetTextureOffsetU(float u);
	void SetTextureOffsetV(float v);
	void SetTextureScaleX(float xScale);
	void SetTextureScaleY(float yScale);

	float GetTextureOffsetU();
	float GetTextureOffsetY();
	float GetTextureScaleX();
	float GetTextureScaleY();
	Math::Vector2 GetTextureOffset();
	Math::Vector2 GetTextureScale();
};

//////////////////////////////////

%template(MaterialAttribNodeVector)std::vector < MatAttribNode* > ;
%template(MaterialAttribNodeMap)std::map<unsigned, std::vector<MatAttribNode*>> ;

class MaterialImpl
{
	MaterialImpl();
	~MaterialImpl();
public:
	typedef std::map<unsigned, std::vector<MatAttribNode*>> AttributeMetaLookup;
	AttributeMetaLookup mAttributeMetaLookup;
};