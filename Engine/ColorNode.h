/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#include "MatAttribNode.h"
#include "AssetManager.h"

class ColorNode : public MatAttribNode
{
public:

	Color color;
  Vector2 mTextureOffset;
  Vector2 mTextureScale;
  static MatAttribNode_Type::TypeEnum static_node_type;

	virtual void BindAttribute(Shader* shader);
	void SetColor(const Color& col);
  void SetTextureOffset(const Vector2& offset);
  void SetTextureScale(const Vector2& scale);
	ColorNode();
	virtual ~ColorNode();
	virtual MatAttribNode* CopyNode();

	___SERIALIZE_BEGIN___
		___DEFBASE(MatAttribNode)
		___DEFSER(color, 1)
		if (ver == 1)
			color = Color();
    ___DEFSER(mTextureScale, 2)
    ___DEFSER(mTextureOffset, 2)
		___SERIALIZE_END___

		metadef(ColorNode)
		m_addbase(MatAttribNode);
		endmetadef;
};
___SERIALIZE_CLASS(ColorNode,2);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change
