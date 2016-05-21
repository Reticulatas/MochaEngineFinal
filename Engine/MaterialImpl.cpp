#include "stdafx.h"
#include "MaterialImpl.h"
#include "ShaderNode.h"

void MaterialImpl::AddAttribute( MatAttribNode* attribute )
{
	if (attribute)
	{
		mAttributeMetaLookup[attribute->mNodeType].push_back(attribute);
	}
}

void MaterialImpl::RemoveAttribute( MatAttribNode* attribute )
{
  std::vector<MatAttribNode*>& vec = mAttributeMetaLookup[attribute->mNodeType];
	mAttributes_iter iter_b = vec.begin();
	mAttributes_iter iter_e = vec.end();
	mAttributes_iter it = std::find(iter_b, iter_e, attribute);
	if (it != vec.end())
	{
		delete(*it);
		vec.erase(it);
		return;
	}
}

Shader* MaterialImpl::GetShader()
{
	ShaderNode* shaderNode = reinterpret_cast<ShaderNode*>(GetAttribute<ShaderNode>());
	if(!shaderNode)
	{
		shaderNode = new ShaderNode();
    shaderNode->SetShader("MochaDeferredGeometry.ms");
		AddAttribute(shaderNode);
	}

	if(!shaderNode)
		return 0;

	return shaderNode->GetShader();
}

void MaterialImpl::BindAttributes(Shader* overrideShader)
{
	Shader* shader = (overrideShader) ? overrideShader : GetShader();

	for (AttributeMetaLookup::iterator it = mAttributeMetaLookup.begin(); it != mAttributeMetaLookup.end(); ++it)
	{
		for (mAttributes_iter mit = it->second.begin(); mit != it->second.end(); ++mit)
			(*mit)->BindAttribute(shader);
	}
}

void MaterialImpl::ClearAttributes()
{
	for (AttributeMetaLookup::iterator it = mAttributeMetaLookup.begin(); it != mAttributeMetaLookup.end(); ++it)
	{
		for (mAttributes_iter mit = it->second.begin(); mit != it->second.end(); ++mit)
			delete (*mit);
	}

	mAttributeMetaLookup.clear();
}

MaterialImpl& MaterialImpl::operator=( const MaterialImpl& rhs )
{
	// avoid self assignment
	if( &rhs == this)
		return *this;

	for (AttributeMetaLookup::const_iterator it = rhs.mAttributeMetaLookup.begin(); it != rhs.mAttributeMetaLookup.end(); ++it)
	{
		for (mAttributes_constiter mit = it->second.begin(); mit != it->second.end(); ++mit)
			AddAttribute((*mit)->CopyNode());
	}

	return *this;
}

unsigned MaterialImpl::GetAttributeCount()
{
	unsigned num = 0;
	for (AttributeMetaLookup::iterator it = mAttributeMetaLookup.begin(); it != mAttributeMetaLookup.end(); ++it)
		num += it->second.size();
	return num;
}

MatAttribNode* MaterialImpl::GetAttribute(unsigned type_id, unsigned index /* = 0 */)
{
	if (mAttributeMetaLookup.find(type_id) == mAttributeMetaLookup.end())
		return 0;
	std::vector<MatAttribNode*>& list = mAttributeMetaLookup[type_id];
	if (index >= list.size())
		return 0;
	return mAttributeMetaLookup[type_id][index];
}
