#pragma once
#include "core.h"
#include "index_seq.h"
#include "seqop.h"

namespace Talg {

	struct ForwardArgInit{};
	struct CharInitListTag{};

	//todo 利用这些来支持string literal的初始化.
	//template<class T,class...Ts>
	//using IsAnyOf = OrValue<std::is_same<T, Ts>...>;
	//
	//template<class T>
	//using IsCharType = IsAnyOf<T, char, signed char, unsigned char, char16_t, char32_t, wchar_t>;

	template<class N,class T,class...Ts>
	struct InitList: InitList<N,Ts...>{
		using Base = InitList<N, Ts...>;
		using Base::Base;
		constexpr InitList(const T& arg,const Ts&...args):Base(ForwardArgInit{},arg,args...){}
		constexpr InitList():Base{}{}
		constexpr size_t size()const noexcept {
			return N::value;
		}
	protected:
		template<class...Us>
		constexpr InitList(ForwardArgInit,const Us&...args):Base{ForwardArgInit{},args...}{}
	};

	template<class Dst,class T,class N,size_t...Is>
	constexpr Dst convertInitListTo(const InitList<N,T>& src, IdSeq<Is...>) {
		return Dst{ src[Is]... };
	}

	template<class N,class T>
	struct InitList<N,T>{
		T data[N::value];
		constexpr InitList(const T& arg):data{arg}{}
		constexpr InitList():data{}{}
		constexpr decltype(auto) operator[](size_t id)const {
			return data[id];
		}
		constexpr size_t size()const noexcept {
			return N::value;
		}
		template<class U>
		constexpr U cast()const{
			return convertInitListTo<U,T,N>(*this, std::make_index_sequence<N::value>());
		}
	protected:
		template<class...Us>
		constexpr InitList(ForwardArgInit,const Us&...args):data{args...}{}
	};

	template<class T,size_t N>
	using InitList_t = Transform_t<
		InitList<Tagi<1>,T>, 
		PushFront_s<
			Tagi<N>, 
			GenSeq_t<N, T>
		>
	>;


}







