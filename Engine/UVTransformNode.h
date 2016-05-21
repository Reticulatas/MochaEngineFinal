/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once
#include "MatAttribNode.h"
#include "MathIncludes.h"

class UVTransformNode : public MatAttribNode
{


	friend class MaterialAttributeNodeComponent;
public:
	Vector2 mTextureOffset;
	Vector2 mTextureScale;

	virtual void BindAttribute(Shader* shader);
	virtual void OnMessage(const std::string& message, void*& param);

	void SetTextureOffset(const Vector2& offset);
	void SetTextureScale(const Vector2& scale);
	void SetTextureOffsetU(float u);
	void SetTextureOffsetV(float v);
	void SetTextureScaleX(float xScale);
	void SetTextureScaleY(float yScale);

	float GetTextureOffsetU();
	float GetTextureOffsetY();
	float GetTextureScaleX();
	float GetTextureScaleY();
	Vector2 GetTextureOffset();
	Vector2 GetTextureScale();

	UVTransformNode();
	virtual ~UVTransformNode();
	virtual MatAttribNode* CopyNode();
	___SERIALIZE_BEGIN___
		___DEFBASE(MatAttribNode)
		___DEFSER(mTextureOffset, 1)
		___DEFSER(mTextureScale, 1)
		___SERIALIZE_END___

		metadef(UVTransformNode)
		m_addbase(MatAttribNode);
	endmetadef;
};
___SERIALIZE_CLASS(UVTransformNode, 1);


/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change