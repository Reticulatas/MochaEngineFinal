#pragma once
#include "retHeader.h"

class RETAPI ISlottable
{
	struct RETAPI Identification
	{
		unsigned m_ID;
		unsigned m_version;
		Identification(unsigned id_, unsigned v_) : m_ID(id_), m_version(v_) { }
	};
	Identification m_id;
	bool m_valid;
public:

	ISlottable( ) : m_id(Identification(UINT_MAX,0)), m_valid(false) { }
	ISlottable( unsigned id_, unsigned version_ ) : m_id(id_, version_), m_valid(true) { }
	virtual ~ISlottable( ) { if (GetIsValidObject()) throw std::exception("Cannot explicitly destroy ISlottable"); }
	ISlottable( const ISlottable& c ) : m_id(c.m_id), m_valid(c.m_valid) { }

	//Returns whether or not this object actively exists in it's slot table
	bool GetIsValidObject( ) { return m_valid; }
	Identification GetIndentification( ) { return m_id; } 

	template <typename T>
	friend class SlotTable;
};


