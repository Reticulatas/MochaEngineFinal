#pragma once
#include "retHeader.h"
#include "ISlottable.h"
#include "managedPtr.h"

/*
SlotTable is a pooled data structure for re-using objects after their lives have expired.

- Type T must extend from ISlottable
- Stored type T must be dynamic memory allocated

New/Delete - log(n)
Iteration  - n + map overhead
Random access by ID only see ISlottable::GetIdentification
*/

template <typename T>
class RETAPI SlotTable
{
	std::vector<T*> m_objectList;			//the full vector of objects, valid or invalid
	std::list<T*>	m_freeList;				//list of invalid objects waiting to be reassigned
	std::set<T*>	m_allocated;			//map of valid objects for iteration (lg(n) insert/delete)
	unsigned		m_numOfObjects;			//size of the allocatedList

public:
	//Constructor
	SlotTable( ) : m_numOfObjects(0) 
	{
	}

	//Destructor
	virtual ~SlotTable( )
	{
		Clear();
	}

	enum SlotTableError
	{
		ST_NOERROR,
		ST_DOUBLEDELETE,
		ST_DOUBLEADDITION
	};

	//Add a new object to the slot table which has been allocated elsewhere.
	//The slot table now controls the object's memory and the client should relinquish hold of it
	//This is also a helper for New, so transient changes only occur during the registration to the slot table
	SlotTableError NewManaged( T* obj )
	{
		if (m_allocated.find(obj) != m_allocated.end())
			return SlotTableError::ST_DOUBLEADDITION;
		m_objectList.push_back(obj);
		ISlottable* slot = dynamic_cast<ISlottable*>(obj);
		slot->m_id.m_ID = m_objectList.size() - 1;
		slot->m_id.m_version = 1;
		slot->m_valid = true;

		m_allocated.insert(obj);
		++m_numOfObjects;
		return SlotTableError::ST_NOERROR;
	}

	//Retrieve new instance of object T, with optional constructor arguments
	template <typename ...Args>
	T* New( Args... args )
	{
		T* obj = 0;

		if (m_freeList.empty())											//no object to placement, make new
		{
			obj = new T( std::forward<Args>(args)... );
			NewManaged(obj);
		}
		else															//placement object exists
		{
			T* placement = m_freeList.back();
			m_freeList.pop_back();
			ISlottable* slot = dynamic_cast<ISlottable*>(placement);	//retrieve old settings
			unsigned oldID = slot->m_id.m_ID;
			unsigned oldVersion = slot->m_id.m_version;
			obj = new(placement) T( std::forward<Args&>(args)... );
			slot = dynamic_cast<ISlottable*>(obj);						//set the old settings to the new slot
			slot->m_id.m_ID = oldID;
			slot->m_id.m_version = ++oldVersion;
			slot->m_valid = true;

			m_allocated.insert(obj);
			++m_numOfObjects;
		}
		//do nothing here, NewManaged must remain valid before this point//
		return obj;
	}

	//Relinquish instance of object T
	SlotTableError Delete ( T* obj )
	{
		ISlottable* slot = dynamic_cast<ISlottable*>(obj);
		if (!slot->GetIsValidObject())
			return ST_DOUBLEDELETE;
		slot->m_valid = false;
		obj->~T();

		m_allocated.erase(obj);
		m_freeList.push_back(obj);

		--m_numOfObjects;
		return ST_NOERROR;
	}

	SlotTableError Delete( std::list<T*> list )
	{
		for (T*& o : list)
			Delete(o);
	}

	//Clear the table, destroying all objects but not resetting the table
	void Clear( )
	{
		for (auto& o : m_objectList)
			if (dynamic_cast<ISlottable*>(o)->GetIsValidObject())
				Delete(o);
		m_numOfObjects = 0;
		m_allocated.clear();
	}

	//Reset the table, destroying all objects and freeing used memory, effectively starting the structure over again
	void Reset( )
	{
		Clear();
		m_objectList.clear();
		m_freeList.clear();
	}

	//Returns an object, given it's identification information. If no object is found or object is invalid, returns 0
	T* GetObject( ISlottable::Identification& id_)
	{
		T* obj = m_objectList->operator[](id_);
		ISlottable* slot = dynamic_cast<ISlottable*>(obj);
		if (!slot->GetIsValidObject())
			return 0;

		if (id_ == slot->m_id)
			return obj;
		return 0;
	}

	//Returns number of objects
	unsigned GetNumberOfObjects( ) { return m_numOfObjects; }

	//Get map for iteration
	std::set<T*>& operator*() { return m_allocated; }
	std::set<T*>* GetAllocatedObjects() { return  &m_allocated; }

#ifdef RETBOOSTSERIALIZATION
	friend class ::boost::serialization::access;
	template<class Archive>
	void save(Archive& archive_, const unsigned int ver) const {
		try {
			std::list<T*> validObjects(m_allocated.begin(), m_allocated.end());
			archive_ & BOOST_SERIALIZATION_NVP(validObjects);
			archive_ & BOOST_SERIALIZATION_NVP(m_numOfObjects);
		}catch(boost::archive::archive_exception& e)	{
			throw "SlotTable serialization data corrupted" + std::string(e.what());
		}
	}
	template<class Archive>
	void load(Archive& archive_, const unsigned int ver) {
		//try {
			if (ver == 0)
			{
				archive_ & BOOST_SERIALIZATION_NVP(m_allocated);
				archive_ & BOOST_SERIALIZATION_NVP(m_freeList);
				archive_ & BOOST_SERIALIZATION_NVP(m_objectList);
				archive_ & BOOST_SERIALIZATION_NVP(m_numOfObjects);
			}
			else
			{
				std::list<T*> validObjects;
				archive_ & BOOST_SERIALIZATION_NVP(validObjects);
				for (T*const& obj : validObjects)
				{
					if (NewManaged(obj) != ST_NOERROR)
						throw std::exception("SlotTable failed to serialize an object");
				}
				archive_ & BOOST_SERIALIZATION_NVP(m_numOfObjects);
			}
		//}catch(boost::archive::archive_exception& e)	{
	//		throw "SlotTable serialization data corrupted" + std::string(e.what());
		//}
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif
};
/*_____________________________________________________________________________________________________*/