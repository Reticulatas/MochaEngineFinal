#pragma once
#include "meta.h"
#include "metaany.h"

namespace meta
{
	namespace helper
	{
		//base
		template <typename T>
		struct get_return_type;

		template <>
		struct get_return_type<void>
		{
			typedef void type;
		};

		template <typename T>
		struct get_return_type : get_return_type<void>
		{
		};

		//naive
		template <typename R, typename... A>
		struct get_return_type<R(*)(A...)>
		{
			typedef R type;
		};

		//variadic specialization
		template <typename R, typename... A>
		struct get_return_type<R(*)(A..., ...)>
		{
			typedef R type;
		};

		//class naive
		template <typename R, typename C, typename... A>
		struct get_return_type<R(C::*)(A...)>
		{
			typedef R type;
		};

		//class variadic
		template <typename R, typename C, typename... A>
		struct get_return_type<R(C::*)(A..., ...)>
		{
			typedef R type;
		};

		//const class naive
		template <typename R, typename C, typename... A>
		struct get_return_type<R(C::*)(A...) const>
		{
			typedef R type;
		};

		//const class variadic
		template <typename R, typename C, typename... A>
		struct get_return_type<R(C::*)(A..., ...) const>
		{
			typedef R type;
		};

		//////////////////////////////////////////////////////////////////////////

		template <size_t i, typename... Args>
		struct get_type_list;

		//Recursive
		template <size_t i, typename Curr, typename... Args>
		struct get_type_list<i, Curr, Args...>
		{
			typedef typename get_type_list<i - 1, Args...>::type type;
		};

		//Base invalid case
		template <size_t i>
		struct get_type_list<i>
		{
			typedef void type;
		};

		//Base valid case
		template <typename Curr, typename... Args>
		struct get_type_list<0, Curr, Args...>
		{
			typedef Curr type;
		};

		//////////////////////////////////////////////////////////////////////////

		//base
		template <typename T, size_t i>
		struct get_arg_type
		{
			typedef void type;
		};
		//static
		template <size_t i, typename Ret, typename... Args>
		struct get_arg_type<Ret(*)(Args...), i>
		{
			typedef  typename get_type_list<i, Args...>::type type;
		};
		template <size_t i, typename Ret, typename... Args>
		struct get_arg_type<Ret(*)(Args..., ...), i>
		{
			typedef  typename get_type_list<i, Args...>::type type;
		};
		//class
		template <size_t i, typename Ret, typename C, typename... Args>
		struct get_arg_type<Ret(C::*)(Args...), i>
		{
			typedef  typename get_type_list<i, Args...>::type type;
		};
		template <size_t i, typename Ret, typename C, typename... Args>
		struct get_arg_type<Ret(C::*)(Args..., ...), i>
		{
			typedef  typename get_type_list<i, Args...>::type type;
		};
		//const class
		template <size_t i, typename Ret, typename C, typename... Args>
		struct get_arg_type<Ret(C::*)(Args...) const, i>
		{
			typedef  typename get_type_list<i, Args...>::type type;
		};
		template <size_t i, typename Ret, typename C, typename... Args>
		struct get_arg_type<Ret(C::*)(Args..., ...) const, i>
		{
			typedef  typename get_type_list<i, Args...>::type type;
		};

		//////////////////////////////////////////////////////////////////////////

		//base
		template <typename T>
		struct get_arg_count : std::integral_constant<size_t, 0>
		{};
		//static
		template <typename R, typename... Args>
		struct get_arg_count<R(*)(Args...)> : std::integral_constant<size_t, sizeof...(Args)>
		{};
		template <typename R, typename... Args>
		struct get_arg_count<R(*)(Args..., ...)> : std::integral_constant<size_t, sizeof...(Args)>
		{};
		//class
		template <typename R, typename C, typename... Args>
		struct get_arg_count<R(C::*)(Args...)> : std::integral_constant<size_t, sizeof...(Args)>
		{};
		template <typename R, typename C, typename... Args>
		struct get_arg_count<R(C::*)(Args..., ...)> : std::integral_constant<size_t, sizeof...(Args)>
		{};
		//const class
		template <typename R, typename C, typename... Args>
		struct get_arg_count<R(C::*)(Args...) const> : std::integral_constant<size_t, sizeof...(Args)>
		{};
		template <typename R, typename C, typename... Args>
		struct get_arg_count<R(C::*)(Args..., ...) const> : std::integral_constant<size_t, sizeof...(Args)>
		{};

		//////////////////////////////////////////////////////////////////////////

		struct parameter
		{
			any_val _data;
            template <typename T>
			parameter(T val) { _data = meta::makeany_val<T>::make(val); }
			parameter(any_val& v) { _data = v; }

			parameter& operator=(parameter&& o) { _data = o._data; return *this; }
			parameter(parameter& o) { _data = o._data; }
			parameter& operator=(parameter& o) { _data = o._data; return *this; }
		};

		struct paramater_pack
		{
			std::vector<parameter> params;
		};

	}
}
//safety