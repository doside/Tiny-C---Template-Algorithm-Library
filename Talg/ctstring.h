#pragma once
#include "core.h"
#include "index_seq.h"
#include "fold.h"
#include "type_traits.h"
#include "fold.h"
#include "functional.h"
#include "initlist.h"

namespace Talg{

	template<class T,size_t N>
	struct ctArray{
		using IdType = std::make_index_sequence<N>;
		InitList_t<T, N> data;
		constexpr ctArray(const ctArray&)noexcept = default;
		constexpr ctArray()noexcept:data{}{}
		//template<class...Ts,class=
		//	std::enable_if_t<
		//		AndValue<IsDecaySame<T,Ts>...>::value
		//	>
		//>
		//constexpr ctArray(Ts&&...args):data{forward_m(args)...}{}
		constexpr ctArray(const InitList_t<T,N>& list)
			:data(list){}
		template<size_t L,size_t L2,size_t...I,size_t...I2>
		constexpr ctArray(const ctArray<T,L>& lhs,const ctArray<T,L2>& rhs,IdSeq<I...>,IdSeq<I2...>)
			:data{lhs[I]...,rhs[I2]...}{}

		constexpr decltype(auto) operator[](size_t id)const {
			return data[id];
		}
		template<size_t M>
		constexpr bool operator==(const ctArray<T, M>& rhs)const {
			return false;
		}
		constexpr bool operator==(const ctArray& rhs)const {
			return compare(rhs, std::make_index_sequence<N>());
		}
		template<size_t... I>
		constexpr bool compare(const ctArray& rhs,IdSeq<I...>)const{
			return foldl(logical_and{}, data[I] == rhs[I]...);
		}
		template<size_t M>
		constexpr bool operator!=(const ctArray<T, M>& rhs)const {
			return !(operator==(rhs));
		}
		
	};
	template<class T,size_t L,size_t M>
	constexpr auto operator+(const ctArray<T,L>& lhs,const ctArray<T,M>& rhs){
		return ctArray<T, M + L>(lhs, rhs, std::make_index_sequence<L>(), std::make_index_sequence<M>());
	}
	template<class T,size_t N,size_t Len,size_t... I>
	constexpr auto slice(const ctArray<T,N>& lhs,Tagi<Len>,IdSeq<I...>){
		return ctArray<T, Len>{{lhs[I]...}};
	}
	template<size_t M,class T,size_t N>
	constexpr auto slice(const ctArray<T,N>& lhs){
		return slice(lhs,Tagi<M>(),std::make_index_sequence<M>());
	}



	template<size_t Len>
	class ctString {
	public:
		using size_type = std::size_t;
		static constexpr size_type npos = no_index;
		ctArray<char, Len> data;
	private:
		template<size_t L,size_t...Index>
		constexpr ctString(const char (&str)[L], IdSeq<Index...>&&)noexcept
		:data{{str[Index]...}} {}
		constexpr ctString(const ctArray<char,Len>& rhs)noexcept:data(rhs){}
	public:	
		constexpr ctString(const ctString<Len>&args) = default;

		template<size_t M,size_t L>
		constexpr ctString(const ctString<M>& lhs,const ctString<L>& rhs)
		:data(slice<M-1>(lhs.data) + rhs.data){}
	
		template<size_t N>
		constexpr ctString(const char(&str)[N])
		:ctString(str,std::make_index_sequence<N>()){}
	
		constexpr decltype(auto) operator[](size_type index)const  {
			return data[index];
		}

		constexpr bool operator==(const ctString& rhs)const noexcept {
			return data == rhs.data;
		}
		template<size_t M>
		constexpr bool operator==(const ctString<M>& rhs)const noexcept {
			return false;
		}
		template<size_t M>
		constexpr bool operator!=(const ctString<M>& rhs)const noexcept {
			return !(*this == rhs);
		}
		constexpr auto size()const noexcept { return Len; }
		constexpr auto length()const noexcept { return Len; }

		//MSVC bug
		//template<size_t M>
		//constexpr auto append(const ctString<M>& rhs)const {
		//	return *this + rhs;
		//}
	};

	template<size_t L,size_t L2>
	constexpr auto operator+(const ctString<L>& lhs, const ctString<L2>& rhs)noexcept {
		return ctString<L + L2 - 1>(lhs, rhs);
	}
}//namespace Talg
