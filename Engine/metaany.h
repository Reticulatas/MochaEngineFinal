#pragma once
#include <type_traits>
#include <typeinfo>
#include "meta.h"

//  meta definition for an 'any' object capable of storing values or references //

class any
{
public:
	const meta::metaobj* _metadata;
	virtual bool GetIsReference() = 0;
	any() : _metadata(0) {}
};

class any_val : public any
{
	struct val_store
	{
		void* val_ref;

		~val_store() {
			delete val_ref;
		}
	};
	template <typename T>
	struct val_store_impl : public val_store
	{
		val_store_impl(const T& v)
		{
			static_assert(std::is_copy_constructible<T>::value, "This any value must have copy constructor.");
			val_ref = new T(v);
		}
	};

	union data
	{
		char _char;
		unsigned char _uchar;
		int _int;
		unsigned _uint;
		short _short;
		unsigned short _ushort;
		long _long;
		unsigned long _ulong;
		float _float;
		double _double;
		long double _ldouble;
		void* _ref;                 //user data

        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, char>::value == true, bool>::type = false>
		CASTTYPE cast() { return _char; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, unsigned char>::value == true, bool>::type = false>
		CASTTYPE cast() { return _uchar; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, int>::value == true, bool>::type = false>
		CASTTYPE cast() { return _int; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, unsigned>::value == true, bool>::type = false>
		CASTTYPE cast() { return _uint; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, short>::value == true, bool>::type = false>
		CASTTYPE cast() { return _short; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, unsigned short>::value == true, bool>::type = false>
		CASTTYPE cast() { return _ushort; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, long>::value == true, bool>::type = false>
		CASTTYPE cast() { return _long; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, unsigned long>::value == true, bool>::type = false>
		CASTTYPE cast() { return _ulong; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, float>::value == true, bool>::type = false>
		CASTTYPE cast() { return _float; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, double>::value == true, bool>::type = false>
		CASTTYPE cast() { return _double; }
        template <typename CASTTYPE, typename std::enable_if<std::is_same<CASTTYPE, long double>::value == true, bool>::type = false>
		CASTTYPE cast() { return _ldouble; }
		template <typename CASTTYPE, typename std::enable_if<std::is_pointer<CASTTYPE>::value == true && std::is_same<CASTTYPE, const char*>::value == false, bool>::type = false>
		CASTTYPE cast() { return reinterpret_cast<CASTTYPE>(_ref); }
        template <typename CASTTYPE, typename std::enable_if<std::is_class<CASTTYPE>::value == true, bool>::type = false>
		CASTTYPE cast() { return *reinterpret_cast<CASTTYPE*>(_ref); }
        template <typename CASTTYPE, typename std::enable_if<std::is_reference<CASTTYPE>::value == true, bool>::type = false>
		CASTTYPE cast() { return *reinterpret_cast<std::remove_reference<CASTTYPE>::type*>(_ref); }

        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, char>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _char = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, unsigned char>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _uchar = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, int>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _int = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, unsigned>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _uint = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, short>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _short = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, unsigned short>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _ushort = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, long>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _long = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, unsigned long>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _ulong = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, float>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _float = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, double>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _double = v; }
        template <typename SETTYPE, typename std::enable_if<std::is_same<SETTYPE, long double>::value == true, bool>::type = false>
		void set(const SETTYPE v) { _ldouble = v; }
		template <typename SETTYPE, typename std::enable_if<std::is_pointer<SETTYPE>::value == true && std::is_same<SETTYPE, const char*>::value == false, bool>::type = false>
		void set(const SETTYPE v) {
			_ref = const_cast<std::remove_const<SETTYPE>::type>(v); 
		}
        template <typename SETTYPE, typename std::enable_if<std::is_class<SETTYPE>::value == true, bool>::type = false>
		void set(const SETTYPE v) { static_assert<std::is_copy_constructible<SETTYPE>::value>("Cannot make a value any from an object with explicit copy");  _ref = v; }
	};
	data _data;
	bool _userdata;
	val_store* _store;

public:
	any_val() : _userdata(false),_store(0), any() {}
	~any_val() 
	{
		if (_userdata) {
			//delete _data._ref; 
			_data._ref = 0; 
		}
		if (_store) {
			//delete _store;
			_store = 0;
		}
	}

    //indicates transfer of ownership
	any_val(any_val&& other);
	any_val& operator=(any_val& other);
	any_val& operator=(any_val&& other);
	friend void copy_any_val(any_val& to, any_val& other);

    //If given a user class, the data will be copied as if it was POD
    template <class ValueType, typename std::enable_if<std::is_class<ValueType>::value == true && std::is_reference<ValueType>::value == false && std::has_trivial_copy<ValueType>::value == true, bool>::type = false>
	void init(const ValueType& v)
	{
		static_assert(std::is_copy_constructible<ValueType>::value,"Cannot make a value any from an object with explicit copy");
		_data._ref = new ValueType(v);
		_userdata = true;

		_metadata = &meta::database::gettypefromT<ValueType>().getmeta();
	}
	template <class ValueType, typename std::enable_if<std::is_class<ValueType>::value == false && std::is_reference<ValueType>::value == false && std::has_trivial_copy<ValueType>::value == true, bool>::type = false>
    void init(const ValueType& v)
	{ 
		_userdata = false;
		if (!std::is_pod<ValueType>::value)
			throw std::exception("Cannot create any_val from non-pod type");
        
		_data.set<ValueType>(v);
		_metadata = &meta::database::gettypefromT<ValueType>().getmeta();
	}
	//reference
	template <class ValueType, typename std::enable_if<std::is_class<ValueType>::value == true && std::is_reference<ValueType>::value == true && std::has_trivial_copy<ValueType>::value == true, bool>::type = false>
	void init(const ValueType v)
	{
		//static_assert(std::is_copy_constructible<ValueType>::value,"Cannot make a value any from an object with explicit copy");
		_data._ref = &v;
		_userdata = true;

		_metadata = &meta::database::gettypefromT<std::remove_reference<ValueType>::type>().getmeta();
	}
	//string special case
	template <class ValueType, typename std::enable_if<std::has_trivial_copy<ValueType>::value == false, bool>::type = false>
	void init(const ValueType v)
	{
		//static_assert(std::is_copy_constructible<ValueType>::value,"Cannot make a value any from an object with explicit copy");
		_store = new val_store_impl<ValueType>(v);
		_data._ref = _store->val_ref;
		_userdata = true;

		_metadata = &meta::database::gettypefromT<std::remove_reference<ValueType>::type>().getmeta();
	}

	template <class ValueType, typename std::enable_if<std::is_void<ValueType>::value == true, bool>::type = false>
	void init(void)
	{
		_userdata = false;
		_metadata = 0;
	}

	bool Valid() { return (_metadata != 0); }
	bool GetIsReference() override { return false; }
    
    template <typename T, typename std::enable_if<std::is_reference<T>::value == true,bool>::type = false>
	T& GetAs()
	{
		if (_userdata)
		{
			/*if (std::is_pointer<T>::value == false)
				throw std::exception("User Data any must be retrieved as pointer type");*/
			return *reinterpret_cast<std::remove_reference<T>::type*>(_data._ref);
		}
		return _data.cast<T>();
	}
    template <typename T, typename std::enable_if<std::is_reference<T>::value == false,bool>::type = false>
	T GetAs()
	{
		if (_store)
		{
			return *_data.cast<T*>();
		}
		else if (_userdata)
		{
			if (std::is_pointer<T>::value == false)
				throw std::exception("User Data any must be retrieved as pointer type");
			//return reinterpret_cast<T>(_data._ref);
		}
		return _data.cast<T>();
	}
};

class any_ref : public any
{
	void* _data;

public:

    template <typename ValueType>
	any_ref(const ValueType* v)
	{
		_data = v;
	}

	bool GetIsReference() override { return true; }

};

namespace meta
{
	template <typename T>
	struct makeany_val;

	template <>
	struct makeany_val < void >
	{
		static any_val make()
		{
			any_val a;
			return a;
		}
	};

	template <typename T>
	struct makeany_val
	{
		static any_val make(T val)
		{
			any_val a;
			a.init<T>(val);
			return a;
		}
	};

	template <typename T>
	static any_val wrap_make_any_val(T val)
	{
		any_val a;
		a.init<T>(val);
		return a;
	}
}