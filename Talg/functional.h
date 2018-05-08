#pragma once
#include "core.h"
#include <type_traits>
#include "basic_macro_impl.h"
#include <functional>
/*!
	\module	Constexpr::Predicator
	与标准库的相关设施的不同点在于返回值的自动推导.
*/
namespace Talg {
	struct logical_and {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const 
		except_when_m(forward_m(lhs) && forward_m(rhs))
		->decltype(forward_m(lhs) && forward_m(rhs))
		{
			return forward_m(lhs) && forward_m(rhs);
		}
	};
	struct logical_or {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const 
		except_when_m(forward_m(lhs) || forward_m(rhs))
		->decltype(forward_m(lhs) || forward_m(rhs))
		{
			return forward_m(lhs) || forward_m(rhs);
		}
	};
	struct plus {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const 
		except_when_m(forward_m(lhs) + forward_m(rhs))
		->decltype(forward_m(lhs) + forward_m(rhs))
		//use trailing return type to supress the MSVC warning C4552
		{			
			return forward_m(lhs) + forward_m(rhs);
		}
	};
	struct minus {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const
		except_when_m(forward_m(lhs) - forward_m(rhs))
		->decltype(forward_m(lhs) - forward_m(rhs))
		//use trailing return type to supress the MSVC warning C4552
		{
			return forward_m(lhs) - forward_m(rhs);
		}
	};
	struct equal_to {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const 
		except_when_m(forward_m(lhs) == forward_m(rhs))
		->decltype(forward_m(lhs) == forward_m(rhs))
		//use trailing return type to supress the MSVC warning C4552
		{
			return forward_m(lhs) == forward_m(rhs);
		}
	};

}

#include "undef_macro.h"

