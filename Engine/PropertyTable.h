/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include <list>
#include <map>

/*!
 *	A property table contains a table that is similar to a hash table, though fills a different use.
 *	Data structure looks as follows:
 *	 | Property | -> [ list of elements]
 *	 | Property | -> [ list of elements]
 *	 | Property | -> [ list of elements]
 *
 *	You may query the table to retrieve the list of elements with a certain property.
 *	Properties:
 *		- No duplicates within a list
 *		- Duplicates allowed between lists
 *		- Near O(1) for list return
 *		- NLogN for insertion and deletion
 *		- Throws std::exceptions on duplicates found
 *		- No manual allocated data (transient only)
*/

#define PROPTABLESAFEMODE

template <typename PropertyType, typename ElementType>
class PropertyTable
{
private:
	std::map<PropertyType,std::list<ElementType>> table;
	typedef typename std::map<PropertyType,std::list<ElementType>>::iterator table_iter;
	typedef typename std::map<PropertyType,std::list<ElementType>>::const_iterator table_const_iter;
	typedef typename std::pair<PropertyType, std::list<ElementType>> table_pair;
	typedef typename std::list<ElementType>::iterator table_list_iter;
	typedef typename std::list<ElementType>::const_iterator table_list_const_iter;
	unsigned mElementCount;
public:
	//Constructor
	PropertyTable() : mElementCount(0) {}

	//Get a linked list of all elements that have a certain property
	//Returns false if property is not found
	//The return list is in the form of a pointer to a list of the element type: out_list_ptr
	bool GetList( const PropertyType& prop_, const std::list<ElementType>*& out_list_ptr ) const;

	//Add an element with a certain property (property will be created if it doesn't exist)
	void Insert(const PropertyType& prop_, const ElementType& element_);

	//Clear the entire table
	void Clear(void);

	//Remove a specific element with a certain property
	//O( elements / properties ) average case
	void Remove(const PropertyType& prop_, const ElementType& element_);
	//Remove all instances of this element
	//O( properties * lg(elements / properties ) )
	unsigned RemoveAll(const ElementType& element_);

	unsigned GetNumberOfProperties(void) const;
	unsigned GetNumberOfElements(PropertyType& prop_) const;
	unsigned GetTotalNumberOfElements(void) const;
};
//-----------------------------------------------------------------------------------------------------

template <typename PropertyType, typename ElementType>
unsigned PropertyTable<PropertyType, ElementType>::GetTotalNumberOfElements( void ) const
{
	return mElementCount;
}

template <typename PropertyType, typename ElementType>
unsigned PropertyTable<PropertyType, ElementType>::GetNumberOfElements( PropertyType& prop_ ) const
{
	table_const_iter iter = table.find(prop_);
	if (iter == table.end())
		throw std::exception("Property not found");
	return iter->second.size();
}

template <typename PropertyType, typename ElementType>
unsigned PropertyTable<PropertyType, ElementType>::GetNumberOfProperties( void ) const
{
	return table.size();
}

template <typename PropertyType, typename ElementType>
void PropertyTable<PropertyType, ElementType>::Remove( const PropertyType& prop_, const ElementType& element_ )
{
	table_iter iter = table.find(prop_);
	if (iter == table.end())
		throw(std::exception("Item not found"));

	 table_list_iter liter = iter->find(element_);
	 if (liter == iter->end())
		throw(std::exception("Item not found"));

	 iter->Remove(liter);

	--mElementCount;
}

template <typename PropertyType, typename ElementType>
unsigned PropertyTable<PropertyType, ElementType>::RemoveAll( const ElementType& element_ )
{
	unsigned removed = 0;

	table_iter iter = table.begin();
	for ( ; iter != table.end(); ++iter )
	{
		table_list_iter liter = iter->second.begin();
		for ( ; liter != iter->second.end(); ++liter )
		{
			if (element_ == *liter)
			{
				++removed;
				liter = iter->second.erase(liter);
				if (liter == iter->second.end())
					break;
			}
		}
	}

	mElementCount -= removed;
	return removed;
}

template <typename PropertyType, typename ElementType>
void PropertyTable<PropertyType, ElementType>::Clear( void )
{
	table.clear();
	mElementCount = 0;
}

template <typename PropertyType, typename ElementType>
void PropertyTable<PropertyType, ElementType>::Insert( const PropertyType& prop_, const ElementType& element_ )
{
	table_iter iter = table.find(prop_);
	if (iter == table.end())
	{
		table.insert(table_pair(prop_, std::list<ElementType>()));
		iter = table.find(prop_);
	}

#ifdef PROPTABLESAFEMODE
	//check for dupes O(n)
	if (std::find(iter->second.begin(), iter->second.end(), element_) != iter->second.end())
		throw std::exception("duplicate property element inserted");
#endif
		
	iter->second.push_back(element_);
	++mElementCount;

	//replace with binary search for faster performance
	//iter->second.sort();
}

template <typename PropertyType, typename ElementType>
bool PropertyTable<PropertyType, ElementType>::GetList( const PropertyType& prop_, const std::list<ElementType>*& out_list_ptr ) const
{
	table_const_iter iter = table.find(prop_);
	if (iter == table.end() || table.size() == 0)
		return false;

	out_list_ptr = &iter->second;
	return true;
}
