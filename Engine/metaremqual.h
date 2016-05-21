#pragma once

namespace meta
{
	// RemQual - Strips down qualified types/references/pointers to a single unqualified type
	template <typename T>
	struct RemQual
	{
		typedef T type;
	};
	template <typename T>
	struct RemQual<const T>
	{
		typedef T type;
	};
	template <typename T>
	struct RemQual<T&>
	{
		typedef T type;
	};
	template <typename T>
	struct RemQual<const T&>
	{
		typedef T type;
	};
	template <typename T>
	struct RemQual<T&&>
	{
		typedef T type;
	};
	template <typename T>
	struct RemQual<T *>
	{
		typedef T type;
	};
	template <typename T>
	struct RemQual<const T *>
	{
		typedef T type;
	};
}
