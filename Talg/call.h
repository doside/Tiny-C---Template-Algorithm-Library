#pragma once
#include <Talg/core.h>
#include <Talg/basic_macro_impl.h>
namespace Talg {
	template<class T>
	constexpr decltype(auto) forEach(T&& func) {
		return forward_m(func)();
	}

	template<class T,class...Ts>
	constexpr decltype(auto) forEach(T&& func, Ts&&...next) {
		return (void)(forward_m(func)()),forEach(forward_m(next)...);
	}
	

}

#include <Talg/undef_macro.h>








