#pragma once
#include "metaobjs.h"
#include "metaany.h"
#include "metahelpers.h"
#include <tuple>
#include <sstream>
#include <functional>

namespace meta
{
    //basic implementation for non-storable
    template <typename FDECL, FDECL fncptr>
	struct metafunction
	{
		static typename helper::get_return_type<FDECL>::type invoke()
		{
			return fncptr();
		}

        template <typename... Args>
		static typename helper::get_return_type<FDECL>::type invoke(Args&&... _args)
		{
			return fncptr(std::forward<Args>(_args)...);
		}
	};
    
	struct metafunction_method;
    //redirects for storable metafunction stage 1 redirects
    template <size_t ARGCOUNT>
	struct _stage1_redirector;
    template<>
	struct _stage1_redirector<0> {
		template <typename... Args>
		static any_val _invoke(metafunction_method* mf, void* class_inst, Args&&... args)
		{
			return mf->_stage1_invoke_noparams(class_inst);
		}
	};
    template <size_t ARGCOUNT>
	struct _stage1_redirector {
		template <typename... Args>
		static any_val _invoke(metafunction_method* mf, void* class_inst, Args&&... args)
		{
			return mf->_stage1_invoke_params(class_inst, args...);
		}
	};

    //storage class
	struct metafunction_method : public metaidentifiable
	{
		void* impl;
		size_t numberofargs;
		helper::paramater_pack pack;
		std::string name;
		const meta::metaobj* return_type_info;

	public:
		std::string getname() const { return name; }
		size_t getnumberofargs() const { return numberofargs; }
		const meta::metaobj* getreturntypemeta() const { return return_type_info; }

    bool operator==(const metafunction_method& other)
    {
      bool ret_info = true;
      if (return_type_info && other.return_type_info)
        ret_info = (*getreturntypemeta() == *other.getreturntypemeta());
      return (getname() == other.getname() && getnumberofargs() == other.getnumberofargs() && ret_info);
    }

        template <typename... Args>
		any_val invoke(void* class_inst, Args&&... args)
		{
			return _stage1_redirector<sizeof...(Args)>::_invoke(this, class_inst, args...);
		}

        template <typename... Args>
		any_val _stage1_invoke_params(void* class_inst, Args&&... args)
		{
			if (pack.params.size() != 0)
				throw std::exception("Invoke attempted on function with pre-packed params. Use invoke_with_pack_params instead.");

			any_val anyargs[] { (meta::wrap_make_any_val(args))... };
			_bind(anyargs, sizeof...(args));
			//_bind({ args... });
			if (sizeof...(args) != numberofargs || sizeof...(args) != pack.params.size())
			{
				std::stringstream ss;
				ss << "Argument count mismatch for invoke {" << name << "} Supplied[" << sizeof...(args) << "," << pack.params.size() << "] Needed[" << (numberofargs) << "]";
				throw std::exception(ss.str().c_str());
			}

			return _redirect_invoke(class_inst, pack);
		}

		template <typename... Args>
		void prepack_params(Args&&... args)
		{
			any_val anyargs[] { (meta::wrap_make_any_val(args))... };
			_bind(anyargs, sizeof...(args));
			if (sizeof...(args) != numberofargs || sizeof...(args) != pack.params.size())
			{
				std::stringstream ss;
				ss << "Argument count mismatch for invoke {" << name << "} Supplied[" << sizeof...(args) << "," << pack.params.size() << "] Needed[" << (numberofargs) << "]";
				throw std::exception(ss.str().c_str());
			}
		}

		any_val invoke_with_pack_params(void* class_inst)
		{
			return _redirect_invoke(class_inst, pack);
		}

        template <typename... Args>
		any_val _stage1_invoke_noparams(void* class_inst)
		{
			if (numberofargs != 0)
				throw std::exception("No parameters supplied for function that needs arguments");
			helper::paramater_pack p;
			return _redirect_invoke(class_inst, p);
		}

		void _bind(any_val* list, int n)
		{
            pack.params.clear();
			//for (auto i : list)
			for (int i = 0; i < n; ++i)
				_rbind(list[i]);
		}

        template <typename T>
		void _rbind(T val)
		{
			pack.params.emplace_back(val);
		}
		virtual any_val _redirect_invoke(void* class_inst, helper::paramater_pack& p) = 0;
	};

    //no args statically determined invoke to impl
    template <typename IMPL>
	any_val _proxy_invoke(void* class_inst, helper::paramater_pack& p, typename std::enable_if<std::is_same<typename IMPL::return_type, void>::value>::type* = 0)
	{
		IMPL::invokepack(class_inst, p);
		any_val a = makeany_val<int>::make(0);
		return a;
	}
    //args statically determined invoke to impl
    template <typename IMPL>
	any_val _proxy_invoke(void* class_inst, helper::paramater_pack& p, typename std::enable_if<std::is_same<typename IMPL::return_type, void>::value == false>::type* = 0)
	{
		return makeany_val<IMPL::return_type>::make(IMPL::invokepack(class_inst, p));
	}

    //CRTP wrapper
    template <typename IMPL>
	struct metafunction_method_wrapper : public metafunction_method
	{
		metafunction_method_wrapper() { impl = this; }

		virtual any_val _redirect_invoke(void* class_inst, helper::paramater_pack& p) override
		{
			if (p.params.size() != numberofargs) {
				std::stringstream ss;
				ss << "Parameter mismatch for invoke function {" << name << "} Pack is malformed. Supplied[" << p.params.size() << "] Needed[" << (numberofargs) << "]";
				throw std::exception(ss.str().c_str());
			}

			any_val r = _proxy_invoke<IMPL>(class_inst, p);
			p.params.clear();
			return r;
		}

        template <typename... Args>
		any_val invoke(void* class_inst, Args&&... _args)
		{
			return IMPL::invoke(class_inst, _args...);
		}
	};

    //logic class - instantiate these, auto-wraps to the non-impl
	template <typename CL, typename FDECL, FDECL fncptr>
	struct metafunction_method_impl : public metafunction_method_wrapper<metafunction_method_impl<CL, FDECL, fncptr>>
	{
		typedef typename helper::get_return_type<FDECL>::type return_type;  //static referenced return_type definition
		static const size_t number_of_args = helper::get_arg_count<FDECL>::value;         //static number of args

		metafunction_method_impl(const char* funcname)	{
			numberofargs = helper::get_arg_count<FDECL>::value;
			name = std::string(funcname);
			return_type_info = &meta::database::gettypefromT<return_type>().getmeta();
		}

        //reference
		static typename helper::get_return_type<FDECL>::type  invoke(CL& inst)
		{
			auto f = std::bind(fncptr, inst);
			return f();
		}

        template <typename... Args>
		static typename helper::get_return_type<FDECL>::type  invoke(CL& inst, Args&&... _args)
		{
			auto f = std::bind(fncptr, inst, std::forward<Args>(_args)...);
			return f(std::forward<Args>(_args)...);
		}

        //pointer
		static typename helper::get_return_type<FDECL>::type  invoke(CL* inst)
		{
			auto f = std::bind(fncptr, inst);
			return f();
		}

		template <typename... Args>
		static typename helper::get_return_type<FDECL>::type  invoke(CL* inst, Args&&... _args)
		{
			auto f = std::bind(fncptr, inst, std::forward<Args>(_args)...);
			return f(std::forward<Args>(_args)...);
		}

        //packs redirect
		static typename helper::get_return_type<FDECL>::type invokepack(void* inst, helper::paramater_pack& p)
		{
			return _invokepack<helper::get_arg_count<FDECL>::value>(reinterpret_cast<CL*>(inst), p);
		}

        //packs no args
        template <int size>
		static typename helper::get_return_type<FDECL>::type _invokepack(CL* inst, helper::paramater_pack& p, typename std::enable_if<size == 0, int>::type = 0)	{
			return invoke(inst);
		}
        //packs args
#define PARAMUNPACK(num)\
	 p.params[num]._data.GetAs<helper::get_arg_type<FDECL, num>::type>()

        template <int size>
		static typename helper::get_return_type<FDECL>::type _invokepack(CL* inst, helper::paramater_pack& p, typename std::enable_if<size == 1,int>::type = 0)	{
			return invoke(inst, PARAMUNPACK(0));
		}
		template <int size>
		static typename helper::get_return_type<FDECL>::type _invokepack(CL* inst, helper::paramater_pack& p, typename std::enable_if<size == 2, int>::type = 0)	{
			return invoke(inst, PARAMUNPACK(0), PARAMUNPACK(1));
		}
		template <int size>
		static typename helper::get_return_type<FDECL>::type _invokepack(CL* inst, helper::paramater_pack& p, typename std::enable_if<size == 3, int>::type = 0)	{
			return invoke(inst, PARAMUNPACK(0), PARAMUNPACK(1), PARAMUNPACK(2));
		}
		template <int size>
		static typename helper::get_return_type<FDECL>::type _invokepack(CL* inst, helper::paramater_pack& p, typename std::enable_if<size == 4, int>::type = 0)	{
			return invoke(inst, PARAMUNPACK(0), PARAMUNPACK(1), PARAMUNPACK(2), PARAMUNPACK(3));
		}
		template <int size>
		static typename helper::get_return_type<FDECL>::type _invokepack(CL* inst, helper::paramater_pack& p, typename std::enable_if<size == 5, int>::type = 0)	{
			return invoke(inst, PARAMUNPACK(0), PARAMUNPACK(1), PARAMUNPACK(2), PARAMUNPACK(3), PARAMUNPACK(4));
		}
		template <int size>
		static typename helper::get_return_type<FDECL>::type _invokepack(CL* inst, helper::paramater_pack& p, typename std::enable_if<size == 6, int>::type = 0)	{
			return invoke(inst, PARAMUNPACK(0), PARAMUNPACK(1), PARAMUNPACK(2), PARAMUNPACK(3), PARAMUNPACK(4), PARAMUNPACK(5));
		}
	};
}

/* Helpers */

//meta::metafunction_method<A, decltype(&A::Foo), &A::Foo>
#define ___HELPER_METAMETHODDECL(classname, methodname) ___CONCAT(___CONCAT(___CONCAT(&,classname),::),methodname)
#define MAKEMETAMETHOD(classname, methodname) meta::metafunction_method_impl<classname, decltype(___HELPER_METAMETHODDECL(classname, methodname)), ___HELPER_METAMETHODDECL(classname, methodname)>
#define MAKEMETAMETHODVARIABLE(classname, methodname) reinterpret_cast<meta::metafunction_method*>(new MAKEMETAMETHOD(classname, methodname)(___GETNAME(methodname)))

//safety comment
