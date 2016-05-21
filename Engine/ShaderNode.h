/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Eduardo Chaves
	-* See Footer for Revisions                                      */
/*

	This Shader node is a special node. It has to be included in
	every material. The material class already takes care of it
*/


#pragma once

#include "MatAttribNode.h"
#include "MochaString.h"

class Shader;

class ShaderNode : public MatAttribNode
{
	Shader* mShader;
  std::string mShaderFileName;
	DEPRECATED MochaString mShaderName;

	friend class MaterialAttributeNodeComponent;

public:

  static MatAttribNode_Type::TypeEnum static_node_type;

	void SetShader(const std::string& shaderName);
	void SetShader(const char* shaderName);
	virtual void BindAttribute(Shader* shader);

  Shader* GetShader();

	ShaderNode();
	virtual ~ShaderNode();
	virtual MatAttribNode* CopyNode();

	___SERIALIZE_BEGIN___
		___DEFBASE(MatAttribNode)
		___DEFSER(mShaderName, 1)
    ___DEFSER(mShaderFileName, 2)
    SetShader(mShaderFileName);
	___SERIALIZE_END___

		metadef(ShaderNode)
		m_addbase(MatAttribNode);
		endmetadef;
};
___SERIALIZE_CLASS(ShaderNode, 2);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change