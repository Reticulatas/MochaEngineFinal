#pragma once
#include "retHeader.h"
#include <unordered_map>

/*
	Dynamic Property Table
	Same as the PropertyTable but includes the ability to have runtime-changing properties as well as static properties
*/

/*
template <typename ObjectType>
class DynamicPropertyTableIterator
{
	const DynamicPropertyTable::PropertyList* propertyListObserved;
	DynamicPropertyTable::PropertyList::propertyListIter matchIter;
	DynamicPropertyTable::PropertyList::propertyListIter unmatchIter;

public:
	DynamicPropertyTableIterator(const DynamicPropertyTable::PropertyList* p)
	{
		propertyListObserved = p;
		unmatchIter = p->m_unmatched.end();
		matchIter = p->m_matched.end();
		if (propertyListObserved->m_matched.size())
			matchIter	= p->m_matched.begin();
		if (propertyListObserved->m_unmatched.size())
			unmatchIter = p->m_unmatched.begin();
	}

	ObjectType* operator++()
	{
		++matchIter;
		if (matchIter != propertyListObserved->m_matched.end())
			return *matchIter;
		else
		{
			return 0;
		}
	}
	bool good()
	{
		//if both iters are at the end, we are finally done
		return !(matchIter == propertyListObserved->m_matched.end() && unmatchIter == propertyListObserved->m_unmatched.end());
	}
};

template <typename ObjectType>
class DynamicPropertyTable
{
public:
	//Structures---
	template <typename PropertyType>
	struct PropertyList
	{
		std::set<const ObjectType*> m_matched;
		std::set<const ObjectType*> m_unmatched;
		typedef std::set<const ObjectType*>::const_iterator propertyListIter;
	};
	template <typename PropertyType>
	class Property
	{
		std::list<const Property*> m_subProperties;
		std::map<PropertyType, PropertyList> m_contained;
	public:
		virtual PropertyType& operator()(ObjectType*) = 0;
		bool Verify(ObjectType*& o, PropertyType& p) const { return (this->operator(o) == p); }
		bool Insert(const ObjectType* obj) 
		{
			PropertyType& propType = this->operator()(obj);
			//add matched
			t.second.m_matched[propType] = obj;
			//add to all unmatched
			for (std::pair<PropertyType,PropertyList> t : m_contained)
				if (t.first != propType)
					t.second.m_unmatched[propType] = obj;
			//walk sub properties
			for (const Property* subprops : m_subProperties)
				subprops->Insert(obj);
			return true;
		}
		bool Remove(const ObjectType* obj)
		{
			//find matched and unmatched for this type, delete if found
			for (std::pair<PropertyType,PropertyList> t : m_contained)
				t.second.m_matched.erase(obj);
			//walk sub properties
			for (const Property* subprops : m_subProperties)
				subprops->Remove(obj);
			return true;
		}

		template <typename T>
		virtual const T* AddSubProperty( ) { T* t = new T(); m_subProperties.push_back(t); return t; }
	};
	template <typename PropertyType>
	class SProperty : public Property
	{
	public:
		virtual PropertyType& operator()(const ObjectType*) = 0;
	};
	template <typename PropertyType>
	class DProperty : public Property
	{
	public:
		virtual PropertyType& operator()(const ObjectType*) = 0;
	};
	template <typename PropertyType>
	class SRootProperty : public SProperty
	{
	public:
		virtual PropertyType& operator()(const ObjectType*) { return 0; }
	};

	//Access Methods---

	void Insert(const ObjectType* obj)
	{
		//walk root properties
		for (const Property* root : _rootProperties)
		{
			root->Insert(obj);
		}
	}

	void Remove(const ObjectType* obj)
	{
		//walk root properties
		for (const Property* root : _rootProperties)
		{
			root->Remove(obj);
		}
	}

	DynamicPropertyTableIterator GetList(const std::string id, PropertyType typ) const
	{
		propertyLookupIter propiter = _propertyLookupTable.find(id);
		if (propiter == _propertyLookupTable.end())
			throw std::exception("Property id not found " + id);
		return DynamicPropertyTableIterator(&propiter->second->m_contained[typ]);
	}

	//Creation Methods---

	template <typename T>
	const T* AddRootProperty( std::string id )
	{
		if (_propertyLookupTable.find(id) != _propertyLookupTable.end())
			throw std::exception("Property with ID " + id + " exists");
		const T* t = new T();
		_rootProperties.push_back(t);
		_propertyLookupTable[id] = t;
	}

	template <typename T>
	const T* AddSubProperty(std::string parent_id, std::string id)
	{
		propertyLookupIter parent = _propertyLookupTable.find(parent_id);
		if (parent == _propertyLookupTable.end())
			throw std::exception("Parent property with ID " + id + " does not exist");
		if (_propertyLookupTable.find(id) != _propertyLookupTable.end())
			throw std::exception("Property with ID " + id + " exists");
		const T* t = parent->second->AddSubProperty<T>();
		if (t)
		{
			_propertyLookupTable[id] = t;
			return t;
		}
		return 0;
	}

private:
	//variables
	std::list<const Property*> _rootProperties;
	std::unordered_map<const std::string, const Property*> _propertyLookupTable;
	typedef std::unordered_map<const std::string, const Property*>::const_iterator propertyLookupIter;

	friend class DynamicPropertyTableIterator;
};
*/