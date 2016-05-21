#pragma once
#include <string>
#include <type_traits>
#include <list>
#include <assert.h>
#include "metaremqual.h"
#include "metadatabase.h"
#include "metamacrohelpers.h"
#include "metadef.h"
#include <map>

namespace meta
{
	struct metafunction_method;

	struct metaidentifiable {
		static unsigned _meta_id_store;
		unsigned meta_id;

		typedef std::list<std::string> TagList;
		TagList tags;

		metaidentifiable();

		bool has_tag(std::string& tag) const;
		unsigned get_meta_id() const;
	};

	typedef const type_info* METATYPEID;
	struct metaobj
	{
		char* type_name;
		unsigned type_id;
		size_t type_size;
		METATYPEID typeinfo;

		metaobj(); 
		~metaobj();

        template <typename T>
		void _init(const char* typename_)
		{
			type_size = sizeof(T);
			typeinfo = &typeid(RemQual<T>::type);
			size_t typenamesize = strlen(typename_) + 1;
			type_name = (char*)malloc(typenamesize);
			strcpy_s(type_name, typenamesize, typename_);
			database::gettypelookuptable().emplace(typeinfo->hash_code(), this);
		}

		bool operator==(const metaobj& b) const { return type_id == b.type_id;	}
		bool operator==(const metaobj* b) const { return type_id == b->type_id;	}

		template<typename G> 
		bool getisobjecttype(typename std::enable_if<std::is_base_of<struct IMeta, G>::value == false, IMeta>::type* = 0) const {
			return typeid(G) == *typeinfo;
		}
		template<typename G> 
		bool getisobjecttype(typename std::enable_if<std::is_base_of<struct IMeta, G>::value, IMeta>::type* = 0) const {
			return (G::getmeta() == this);
		}

		virtual bool getispod() const = 0;
	};

	struct metastatic
	{
		metaobj* type_info;
		const char* var_name;
		void* raw_ptr;

		metastatic(void* ptr, metaobj* o, const char* name);
		metastatic(const metastatic& o);
	};

	struct metamember : public metaidentifiable
	{
		metaobj* type_info;
		size_t offset;
		char* var_name;

		metamember(size_t off_, metaobj* _t, const char* name);
		metamember(const metamember& o);

		/*  Get the actual pointer to this structure's internal
		    Since some types may not be immediately data, this does the translation manually 
            NOTE: DOES MANUAL TRANSLATION OF CERTAIN TYPES TO THEIR UNDERLYING DATA, DO NOT ASSUME THIS IS THE SAME PTR
			NOTE: WHEN REFLECTING CONTAINER, BE SURE THAT THE BASE_PTR IS NOT A POINTER->POINTER->ELEMENT */
		const void* get_ptr_to_member_content(const void* base_ptr_to_struct) const;

		/* Same as get_ptr_to_member_content, but does not do any per-type translation */
		const void* get_raw_ptr_to_member(const void* base_ptr_to_struct) const;

		/* Get the actual size of an instance of this object
		    Includes overloads for specific types */
		unsigned get_actual_size_from_base(const void* base_ptr);

		/* Set underlying data */
		void set_content(void* base_ptr, void* data, size_t data_size) const;
	};

	struct metapod : public metaobj
	{
		bool getispod() const { return true; }
	};

	struct metaclass : public metaobj
	{
		typedef std::list<metamember*> MemberList;
		typedef std::list<metamember*>::const_iterator MemberIterator;
		MemberList _members;     //member vars, anything that can be defined generically
		std::unordered_map<std::string, metamember*> _memberslookup;    //slower str->member table
		typedef std::unordered_map<std::string, metamember*>::const_iterator _memberslookupiter;    //slower str->member table
		typedef std::map<std::string, metafunction_method*> FunctionList;
		FunctionList _functions;
        
        typedef	std::map<std::string, metastatic*> StaticList;  //str -> static lookup
		typedef std::map<std::string, metastatic*>::const_iterator _mstaticlookupiter; //no primary list for statics yet
		StaticList _staticslookup;

        //the base class's meta
		const metaclass* _base_info;

		metamember* _lastaddedmember;       //store last added member for tagging simplicity (only one will be valid)
		metafunction_method* _lastaddedfunction;       //store last added function for tagging simplicity (only one will be non-zero)

		metaclass();

        template <typename T>
		metaclass& _add_member(size_t off_, const char* name, const char* typename_)
		{
			_members.push_back(meta::creation::create_metamember<T>(off_,name, typename_));
			_memberslookup.emplace(_members.back()->var_name, _members.back());
			_lastaddedmember = _members.back();
			_lastaddedfunction = 0;
			return *this;
		}

        template <typename T>
		metaclass& _add_static(void* ptrToMem, const char* name, const char* typename_)
		{
			_staticslookup[std::string(name)] = meta::creation::create_metastatic<T>(ptrToMem, name, typename_);
			_lastaddedmember = 0;
			_lastaddedfunction = 0;  //currently no tagging support for statics
			return *this;
		}

        template <typename T>
		metaclass& _add_base()
		{
			_base_info = &T::getmeta();
			if (_base_info == 0)
			{
				T::meta_struct = T::metacreationstruct();
				_base_info = &T::getmeta();
			}
			for (MemberList::const_iterator iter = _base_info->_members.begin(); iter != _base_info->_members.end(); ++iter)
			{
				_members.push_back(new metamember(**iter));
				_memberslookup.emplace(_members.back()->var_name, _members.back());
			}
			return *this;
		}

		template <typename T>
		bool getisderivedfrom() const
		{
			if (getisobjecttype<T>())
				return true;
			const meta::metaclass* base = _base_info;
			while (base)
			{
				if (base->getisderivedfrom<T>())
					return true;
				base = base->_base_info;
			}
			return false;
		}

		metaclass& _add_function(metafunction_method* v);

        //give the last member a tag
		metaclass& _tag_lastmember(const char* tag);

		const metamember* getmember(const char* c) const;
        //noexcept will give you 0 if member does not exist
		const metamember* getmember(const std::string& c) const;
		const MemberList& getmemberslist() const;
		MemberIterator getmembersbegin() const;
		MemberIterator getmembersend() const;

		meta::metafunction_method*  getfunctionbyname(std::string& fncname) const;
		meta::metastatic*           getstaticbyname(std::string& varname) const;

		bool getispod() const;

		std::string gettoplevelname() const;
	};
};