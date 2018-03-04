#pragma once
#include "core.h"
#include "basic_marco_impl.h"

namespace Talg {
	struct logical_and {
		template<class T,class U>
		constexpr decltype(auto) operator()(T&& lhs, U&& rhs)const {
			return forward_m(lhs) && forward_m(rhs);
		}
	};
	struct logical_or {
		template<class T,class U>
		constexpr decltype(auto) operator()(T&& lhs, U&& rhs)const {
			return forward_m(lhs) || forward_m(rhs);
		}
	};
	struct plus {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const 
		->decltype(forward_m(lhs) + forward_m(rhs)){
			return forward_m(lhs) + forward_m(rhs);
		}
	};
	struct minus {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const
		->decltype(forward_m(lhs) - forward_m(rhs)){
			return forward_m(lhs) - forward_m(rhs);
		}
	};
	struct equal_to {
		template<class T,class U>
		constexpr decltype(auto) operator()(T&& lhs, U&& rhs)const {
			return forward_m(lhs) == forward_m(rhs);
		}
	};
}

#include "undef_macro.h"

