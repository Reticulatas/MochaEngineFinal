#pragma once
#include <unordered_map>
#include <type_traits>
#include <typeinfo>
#include "metaremqual.h"


namespace meta
{
	struct metaobj;
	struct metafunction_method;
	struct metamember;

	struct database
	{
		class metaobjhandle
		{
			metaobj* typeinfo;
		public:
			metaobjhandle() : typeinfo(0) {}
			metaobjhandle(metaobj* v) : typeinfo(v) {}
			const metaobj& getmeta() const { return *typeinfo; }
		};

		static unsigned assignedtypeids;
		static std::unordered_map<size_t, metaobjhandle>& gettypelookuptable()
		{
			static std::unordered_map<size_t, metaobjhandle> typeLookupTable;
			return typeLookupTable;
		}

		typedef std::list<metamember*> TagMatchList;
		typedef std::unordered_map<std::string, TagMatchList> TagLookupTable;
		static TagLookupTable& gettaglookuptable()
		{
			static TagLookupTable tagLookupTable;
			return tagLookupTable;
		}
		typedef std::list<metafunction_method*> TagFuncMatchList;
		typedef std::unordered_map<std::string, TagFuncMatchList> TagFuncLookupTable;
		static TagFuncLookupTable& gettagfunclookuptable()
		{
			static TagFuncLookupTable tagLookupTable;
			return tagLookupTable;
		}

		static metaobjhandle& gettypefromtypeinfo(type_info& v) { return gettypelookuptable()[v.hash_code()]; }
		template <typename T>
		static metaobjhandle& gettypefromT() { return gettypelookuptable()[typeid(RemQual<T>::type).hash_code()]; }
	};
}


//end database