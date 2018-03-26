#pragma once
#include <utility> //for std::swap
#include <type_traits>	//for std::decay_t std::is_same std::is_base_of std::enable_if_t
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


	template<class T,class U>
	using IsDecaySame = std::is_same<std::decay_t<T>, std::decay_t<U>>;

	template<class Base,class T>
	using WhenNoBase = std::enable_if_t<std::is_base_of<Base, T>::value>;
	template<class U,class T>
	using WhenDecaySame = std::enable_if_t<IsDecaySame<T, U>::value>;
	
	template<class Require,class T>
	using Constrait = T;

	
	/*!
		\brief	用于简化SFINAE的写法
		\example	forward reference with SFINAE:
					template<class...Ts>
					ctor(Tself<int,Ts>&&...)
					means:
					ctor(int...)
		\note	已经废弃,此做法有明显缺陷:
				clang会误认为右值; workaround: Tself<int,Ts&&>...
				当用在可变参数上时,msvc会无法SFINAE;

				应该改为:
				template<class...Ts,class=WhenDecaySame<Ts,int>>
				ctor(Ts&&...args)
	*/
	template<class T,class Require,class U=std::enable_if_t<Require::value>>
	using Tself[[deprecated]] = T;

	
	
	
	/*!
		\brief	用于简化SFINAE的写法
		\example	forward reference with SFINAE:
					template<class...Ts>
					ctor(ExcludeBase<Base,Ts&&>...)
					means:
					template<class...Ts,class=WhenNoBase<Ts,Base>>
					ctor(Ts&&...)

		\note	已经废弃,此做法有明显缺陷,应该改为:
				template<class...Ts>
				ctor(Constrait<WhenNoBase<Ts,Base>,Ts&&>...args)
	*/
	template<class Base,class T,
		class U=std::enable_if_t<
			std::is_base_of<std::decay_t<Base>, std::decay_t<T>>::value==false
		>
	>
	using ExcludeBase[[deprecated]] = T;
	




	template<class... Ts> 
	struct make_void { 
		using type = void;
	};
	template<class... Ts> 
	using void_t = typename make_void<Ts...>::type;
}