/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "ColorNode.h"
#include "GraphicsInclude.h"


void ColorNode::BindAttribute( Shader* shader )
{
  Matrix4 texScale, texTransform, texTranslate;
  texScale.Scale(mTextureScale.x, mTextureScale.y, 0.0f);
  texTranslate.Translate(mTextureOffset.x, mTextureOffset.y, 0.0f);
  texTransform = texTranslate * texScale;

  ConstantBufferManager::BindMaterialBuffer(color.ToVector4(), texTransform);
}

ColorNode::ColorNode():MatAttribNode(MatAttribNode_Type::COLOR)
{
	color = Color(1,1,1,1);
  mTextureOffset = Vector2(0, 0);
  mTextureScale = Vector2(1.0f, 1.0f);
}

ColorNode::~ColorNode()
{

}

void ColorNode::SetColor( const Color& col )
{
	color = col;
}

MatAttribNode* ColorNode::CopyNode()
{
	ColorNode* node = new ColorNode;
	node->SetColor(this->color);
	node->mNodeType = this->mNodeType;
  node->mTextureOffset = mTextureOffset;
  node->mTextureScale = mTextureScale;

	return reinterpret_cast<MatAttribNode*>(node);
}

void ColorNode::SetTextureOffset(const Vector2& offset)
{
  mTextureOffset = offset;
}

void ColorNode::SetTextureScale(const Vector2& scale)
{
  mTextureScale = scale;
}


MatAttribNode_Type::TypeEnum ColorNode::static_node_type = MatAttribNode_Type::COLOR;

meta_define(ColorNode);
