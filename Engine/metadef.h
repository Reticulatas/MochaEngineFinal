#pragma once
#include "metaobjs.h"
#include "metamacrohelpers.h"
#include "metaenum.h"
#include "IRegisterable.h"

struct IMeta;

namespace meta
{
	namespace base
	{
	}

	namespace creation
	{
		template <typename T>
		meta::metaobj* create_meta(const char* typename_)
		{
			meta::metaobj* m = 0;
			if (std::is_class<T>::value)
			{
				//Class
				m = new meta::metaclass();
			}
			else if (std::is_pod<T>::value)
			{
				//POD
				m = new meta::metapod();
			}
			m->_init<T>(typename_);
			return m;
		}

		template <typename T>
		meta::metamember* create_metamember(size_t offset_, const char* name_, const char* typename_)
		{
			metaobj* o = create_meta<T>(typename_);
			metamember* m = new metamember(offset_, o, name_);
			return m;
		}

		template <typename T>
		meta::metastatic* create_metastatic(void* ptr, const char* name_, const char* typename_)
		{
			metaobj* o = create_meta<T>(typename_);
			metastatic* m = new metastatic(ptr, o, name_);
			return m;
		}
	}
}

//In header:
#define metadef(t) \
	typedef t type;\
	static struct metacreationstruct { \
	meta::metaclass* meta_info;\
	static char base_check_var;\
	metacreationstruct() : meta_info(reinterpret_cast<meta::metaclass*>(meta::creation::create_meta<t>(___GETNAME(t)))) {\
	static_assert(std::is_base_of<IMeta, t>::value, "Error: Meta-enabled class " ___GETNAME(t) " must be derived from IMeta");\
	(*meta_info)

#define metabasedef(t)\
	typedef t type;\
	static struct metacreationstruct { \
	meta::metaclass* meta_info;\
	static const char base_check_var;\
	metacreationstruct() : meta_info(reinterpret_cast<meta::metaclass*>(meta::creation::create_meta<t>(___GETNAME(t)))) {\
	static_assert(std::is_base_of<IMeta, t>::value, "Error: Meta-enabled class " ___GETNAME(t) " must be derived from IMeta");\
	(*meta_info)

#define endmetabasedef \
	;}};\
	static metacreationstruct meta_struct;\
	static_assert(std::is_const<decltype(metacreationstruct::base_check_var)>::value, "Error: Meta-enabled class has mismatched basedef/def tags");\
	static const meta::metaclass& getmeta() { return *meta_struct.meta_info; }\

#define endmetadef \
	;}};\
	static metacreationstruct meta_struct;\
	static_assert(!std::is_const<decltype(metacreationstruct::base_check_var)>::value, "Error: Meta-enabled class has mismatched basedef/def tags");\
	static const meta::metaclass& getmeta() { return *meta_struct.meta_info; }\
	const meta::metaclass& mymeta() const override { return *meta_struct.meta_info; }

//all meta-enabled classes must derive from this
//should be first derived class
struct IMeta
{
	virtual const meta::metaclass& mymeta() const = 0;

	unsigned mytopid() const {
		return mymeta().type_id;
	}
	static unsigned gettopid() {
		return getmeta().type_id;
	}

	std::string mytoplevelname() const {
		return mymeta().gettoplevelname();
	}

	//___SERIALIZE_BEGIN___
		//___SERIALIZE_END___

	metabasedef(IMeta)
		endmetabasedef;
};
//___SERIALIZE_CLASS(IMeta);




#define m_addbase(typ)\
	._add_base<typ>()

#define m_add(typ, t)\
	._add_member<decltype(t)>(offsetof(type, t), ___GETNAME(t), ___GETNAME(typ))

#define m_tag(tag)\
	._tag_lastmember(tag)

#define m_addfunc(func)\
	._add_function(MAKEMETAMETHODVARIABLE(type, func))

#define m_addstatic(typ, t)\
	._add_static<typ>(&___CONCAT(type::,t), ___GETNAME(t), ___GETNAME(typ));

//In CPP:
#define meta_define(t)\
	t::metacreationstruct t::meta_struct;


//safety comment