#pragma once

#include <vector>
#include <map>
#include "MatAttribNode.h"

class MaterialImpl
{
public:
	//-----------------------------------------------------------------------//
	/* /////////////////////// MEMBER VARIABLES /////////////////////////////*/
	//-----------------------------------------------------------------------//
	//std::vector<MatAttribNode*> mAttributes;
	typedef std::map<unsigned, std::vector<MatAttribNode*>> AttributeMetaLookup;
	AttributeMetaLookup mAttributeMetaLookup;

	typedef std::vector<MatAttribNode*>::iterator mAttributes_iter;
	typedef std::vector<MatAttribNode*>::const_iterator mAttributes_constiter;

	//-----------------------------------------------------------------------//
	/* /////////////////////// MEMBER FUNCTIONS /////////////////////////////*/
	//-----------------------------------------------------------------------//

	// Shader Access
	Shader* GetShader();

	// Attribute operations
	void AddAttribute(MatAttribNode* attribute);
	void RemoveAttribute(MatAttribNode* attribute);
	MatAttribNode* GetAttribute(unsigned type_id, unsigned index = 0);
	template <typename T>
	T* GetAttribute(unsigned index = 0)
	{
    static_assert(std::is_base_of<MatAttribNode, T>::value, "Get Attribute must take top level attribute node");
    unsigned id = T::static_node_type;;
		return reinterpret_cast<T*>(GetAttribute(id, index));
	}
	unsigned GetAttributeCount();
	void BindAttributes(Shader* overrideShader = 0);
	void ClearAttributes();

	// Operators
	virtual MaterialImpl& operator=(const MaterialImpl& rhs);

	___SERIALIZE_BEGIN___
	___DEFSER(mAttributeMetaLookup, 1)
	___SERIALIZE_END___
	friend class Material;
	friend class MaterialComponent;
};
___SERIALIZE_CLASS(MaterialImpl, 1);