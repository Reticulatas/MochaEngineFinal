#pragma once

#define MOCHA_META_MAX_ENUM_VALUES 20

namespace meta
{
	namespace enum_ {
	}
}

#define m_addenum(typ)\
	{ typedef typ t;

#define m_enum_val(name)\
	.add_enum(___CONCAT(t::,name), ___GETNAME(name));