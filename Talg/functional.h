#pragma once
#include "core.h"
namespace Talg {
	struct logical_and {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const {
			return forward_m(lhs) && forward_m(rhs);
		}
	};
	struct plus {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const {
			return forward_m(lhs) + forward_m(rhs);
		}
	};
	struct minus {
		template<class T,class U>
		constexpr auto operator()(T&& lhs, U&& rhs)const {
			return forward_m(lhs) - forward_m(rhs);
		}
	};
}
