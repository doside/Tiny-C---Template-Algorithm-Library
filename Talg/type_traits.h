#pragma once
#include <utility> //for std::swap
#include "core.h"

namespace Talg {
	namespace detail {
		using std::swap;
		template <typename T>
		void swap_impl(T &t, T &u) noexcept(noexcept(swap(t, u)));
	}
	
	template<class T>
	struct is_nothrow_swapable
	{
		static constexpr bool value=noexcept(
			detail::swap_impl( std::declval<T&>(),std::declval<T&>() )
		);
		constexpr is_nothrow_swapable() = default;
		constexpr operator bool()const {
			return value;
		}
	};
	

	template<class T,class Require,class U=std::enable_if_t<Require::value>>
	using Tself = T;

	template<class Base,class T,
		class U=std::enable_if_t<
			std::is_base_of<std::decay_t<Base>, std::decay_t<T>>::value==false
		>
	>
	using ExcludeBase = T;

	template<class T,class U>
	using IsDecaySame = std::is_same<std::decay_t<T>, std::decay_t<U>>;

	//deprecated: It doesnot work with clang.
	//template<class U,class T,class = std::enable_if_t<IsDecaySame<T,U>::value>>
	//using LimitTo = T;

	template<class... Ts> 
	struct make_void { 
		using type = void;
	};
	template<class... Ts> 
	using void_t = typename make_void<Ts...>::type;
}