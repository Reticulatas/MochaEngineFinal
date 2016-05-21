/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "UVTransformNode.h"

void UVTransformNode::BindAttribute( Shader* shader )
{
	/* Texture Transformations */
	//XMMATRIX texScale = XMMatrixScaling(mTextureScale.x,mTextureScale.y,1.0f);
	//XMMATRIX texTranslate = XMMatrixTranslation(mTextureOffset.x,mTextureOffset.y,0.0f);
	//XMFLOAT4X4 texTransform;
	//XMStoreFloat4x4(&texTransform,texScale * texTranslate);
	//float* f = reinterpret_cast<float*>(&texTransform);
	//shader->GetEffect()->GetVariableByName(mAttribName.c_str())->AsMatrix()->SetMatrix(f);

  //Matrix4 texScale, texTransform, texTranslate;
  //texScale.Scale(mTextureScale.x, mTextureScale.y, 0.0f);
  //texTranslate.Translate(mTextureOffset.x, mTextureOffset.y, 0.0f);
  //texTransform = texScale * texTranslate;
  //shader->GetEffect()->GetVariableByName(mAttribName.c_str())->AsMatrix()->SetMatrix(texTransform.ToFloats());
}

void UVTransformNode::OnMessage( const std::string& message, void*& param )
{	
	if(message == "SetTextureOffset")
    mTextureOffset = *(reinterpret_cast<Vector2*>(param));

	else if(message == "SetTextureScale")
    mTextureScale = *(reinterpret_cast<Vector2*>(param));
}

UVTransformNode::UVTransformNode() : MatAttribNode(MatAttribNode_Type::UV)
{
  mTextureOffset = Vector2(0, 0);
  mTextureScale = Vector2(1.0f, 1.0f);
}

UVTransformNode::~UVTransformNode()
{

}

void UVTransformNode::SetTextureOffset(const Vector2& offset)
{
	mTextureOffset = offset;
}

void UVTransformNode::SetTextureScale(const Vector2& scale)
{
	mTextureScale = scale;
}

void UVTransformNode::SetTextureOffsetU( float u )
{
	mTextureOffset.x = u;
}

void UVTransformNode::SetTextureOffsetV( float v )
{
	mTextureOffset.y = v;
}

void UVTransformNode::SetTextureScaleX( float xScale )
{
	mTextureOffset.x = xScale;
}

void UVTransformNode::SetTextureScaleY( float yScale )
{
	mTextureScale.y = yScale;
}

float UVTransformNode::GetTextureOffsetU()
{
	return mTextureOffset.x;
}

float UVTransformNode::GetTextureOffsetY()
{
	return mTextureOffset.y;
}

float UVTransformNode::GetTextureScaleX()
{
	return mTextureScale.x;
}

float UVTransformNode::GetTextureScaleY()
{
	return mTextureScale.y;
}

Vector2 UVTransformNode::GetTextureOffset()
{
	return mTextureOffset;
}

Vector2 UVTransformNode::GetTextureScale()
{
	return mTextureScale;
}

MatAttribNode* UVTransformNode::CopyNode()
{
	UVTransformNode* node = new UVTransformNode;
	node->SetTextureOffset(this->GetTextureOffset());
	node->SetTextureScale(this->GetTextureScale());
	node->mNodeType = this->mNodeType;
	return reinterpret_cast<MatAttribNode*>(node);
}

meta_define(UVTransformNode);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change