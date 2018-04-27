#pragma once
#include <type_traits>
#include <Talg/core.h>
#include <Talg/basic_macro_impl.h>
namespace Talg {

	template<class Out_,class T_>
	struct hasOverloadOstream {
		template<class Out,class T>
		static std::true_type call(Seq<decltype(std::declval<Out&>() << std::declval<T>())>*);
		template<class Out,class T>
		static std::false_type call(EatParam);


		template<class Out,class T>
		static std::true_type fcall(Seq<decltype(operator<<(std::declval<Out&>(),std::declval<T>()))>* );
		template<class Out,class T>
		static std::false_type fcall(EatParam);

		static constexpr bool value = decltype(call<Out_, T_>(0))::value || decltype(fcall<Out_,T_>(0))::value;
		constexpr hasOverloadOstream()noexcept{}
		constexpr operator bool()const noexcept{
			return value;
		}
	};
}

#include <Talg/undef_macro.h>
