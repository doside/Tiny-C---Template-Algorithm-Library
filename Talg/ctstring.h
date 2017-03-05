#pragma once
#include "core.h"
#include "index_seq.h"
#include <tuple>

namespace Talg{
template<size_t Len>
struct ctString {
	using size_type = std::size_t;
	static constexpr size_type npos = no_index;
	char data[Len];
	
	
	constexpr ctString(const ctString<Len>&args) = default;

	//template<size_t...Ls>
	//explicit constexpr ctString(const ctString<Ls>&...args)
	//:ctString(append(ctString<Ls>(args)...)){}
	
	template<size_t N>
	constexpr ctString(const char(&str)[N])
	:ctString(str,std::make_index_sequence<N>()){}
	constexpr decltype(auto) operator[](size_type index)const noexcept {
		return data[index];
	}
	template<class U,class... Ts>
	constexpr bool stringEqual(size_t index,const U& rhs)const noexcept{
		return rhs[index] == 0;//== rhs[index];// && (index == 0 ? true : stringEqual(index - 1, lhs, rhs));
	}
	constexpr decltype(auto) operator==(const ctString& rhs)const noexcept {
		return stringEqual(Len-1,rhs);
	}

	constexpr auto size()const noexcept { return Len; }

	template<size_t L,size_t L2>
	friend constexpr ctString<L + L2-1> operator+(const ctString<L>& lhs, const ctString<L2>& rhs)noexcept;

	template<size_t L2>
	constexpr ctString<Len+L2-1> append(const ctString<L2>& rhs)const noexcept{
		return{ data, rhs.data,std::make_index_sequence<Len-1>(),std::make_index_sequence<L2>() };
	}

//private:
	/*
		\brief	
		\param
		\return
		\note	此处用指针的话gcc通不过constexpr编译
	*/
	template<size_t L,size_t...Index>
	constexpr ctString(const char (&str)[L], IdSeq<Index...>&&)
	:data{str[Index]...}{}

	template<size_t L,size_t L2,size_t...Index,size_t...Index2>
	constexpr ctString(const char (&str)[L],const char (&str2)[L2], IdSeq<Index...>&&, IdSeq<Index2...>&&)
	:data{str[Index]...,str2[Index2]...}{}


};

template<size_t L>
constexpr auto append(const ctString<L>& res)noexcept{
	return res;
}
template<size_t L,size_t L2,class...Ts>
constexpr auto append(const ctString<L>& lhs, const ctString<L2>& rhs,const Ts&...next)noexcept{
	return append(lhs+rhs, next...);
}


template<size_t L,size_t L2>
constexpr ctString<L + L2-1> operator+(const ctString<L>& lhs, const ctString<L2>& rhs)noexcept {
	return lhs.append(rhs);
}

}//namespace Talg
