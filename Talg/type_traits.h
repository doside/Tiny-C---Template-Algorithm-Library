#pragma once
#include <utility> //for std::swap

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
}