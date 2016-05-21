/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include <string>
#include "meta.h"
#pragma once

/* base class from where all Material Attribute nodes extend */

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

class Shader;		// Forward declaring shader

class MatAttribNode : public IMeta
{
public:
	
  MatAttribNode_Type::TypeEnum mNodeType;
	virtual void BindAttribute(	Shader* shader);

	virtual ~MatAttribNode();
	MatAttribNode(const MatAttribNode& rhs){}
	MatAttribNode(MatAttribNode_Type::TypeEnum _type) : mNodeType(_type) { }
	MatAttribNode(void){ }

	MatAttribNode_Type::TypeEnum NodeType() const { return mNodeType; }
	virtual MatAttribNode* CopyNode(){ return this;}

	bool operator == (const MatAttribNode& rhs);

	___SERIALIZE_BEGIN___
		___DEFSER(mNodeType, 1)
		___SERIALIZE_END___

		metabasedef(MatAttribNode)
		endmetabasedef;
};
___SERIALIZE_CLASS(MatAttribNode, 1);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(MatAttribNode);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change