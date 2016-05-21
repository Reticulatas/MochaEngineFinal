#include "stdafx.h"
#include "metaany.h"
#include "meta.h"
#include "metaobjs.h"

void copy_any_val(any_val& to, any_val& other)
{
	to._metadata = other._metadata;
	to._userdata = other._userdata;

	if (other._store)
	{
		to._store = other._store;
		other._store = 0;
	}
	else
		to._store = 0;

	if (other._userdata)
	{
		to._data._ref = other._data._ref;// malloc(_metadata->type_size);
		other._data._ref = 0;
		other._metadata = 0;
		other._userdata = false;
		//memcpy(_data._ref, other._data._ref, _metadata->type_size);
	}
	else
		to._data = other._data;
}

any_val::any_val(any_val&& other)
{
	copy_any_val(*this, other);
}

any_val& any_val::operator=(any_val& other)
{
	copy_any_val(*this, other);
	return *this;
}

any_val& any_val::operator=(any_val&& other)
{
	copy_any_val(*this, other);
	return *this;
}
