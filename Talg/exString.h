#pragma once
#include <functional>
#include <algorithm>
#include "core.h"
#include "find_val.h"  //for no_index
#include <array>


template<class T>
constexpr T min(const T& a, const T& b) {
	return b < a ? b : a;
}
template<class T> 
constexpr T max(const T& a, const T& b)
{
	return a > b ? a : b;
}


template<char...Cs>
struct CharSeq {
	using size_type = std::size_t;
	static constexpr size_type npos = no_index;
	/*template<class Receiver,class Output,class...Ts>
	call(Receiver&& res,Output&& print,Seq<Ts...>&&) {
		return	forward_m(res)(
					forward_m(print)(	
						Ts::call(Cs...).width,
						Ts::call(Cs...).precision 
					)...
				);
	}*/
};

struct exStringView;


template<size_t Len>
struct ctString {
	using size_type = std::size_t;
	static constexpr size_type npos = no_index;
	char data[Len];
	

	template<size_t...Ls>
	explicit constexpr ctString(const ctString<Ls>&...args)
	:ctString(append(ctString<Ls>(args)...)){}
	template<size_t N>
	constexpr ctString(const char(&str)[N])
	:ctString(str,std::make_index_sequence<N>()){}


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

struct exStringView{
public:
	using size_type = std::size_t;
	static constexpr size_type npos = no_index;
private:
	const char* data;
	size_type sz;
	constexpr bool equal(const exStringView& rhs,size_type pos=0)const noexcept {
		return  pos >= min(sz,rhs.sz) ? true : data[pos] == rhs[pos] && equal(rhs, pos + 1);
	}
	constexpr bool compare_less(const exStringView& rhs,size_type pos = 0)const noexcept {
		return  pos >= sz ? 
					false : 
					data[pos] == rhs[pos]?
						compare_less(rhs,pos + 1):
						data[pos] < rhs[pos];
	}

public:
	template<size_type N>
	constexpr exStringView(const char (&str)[N]):data(str),sz(N-1){}
	constexpr exStringView(const char* str, size_type len):data(str),sz(len){}

	constexpr char operator[](size_type index)const noexcept { return data[index]; }
	constexpr size_type size()const noexcept { return sz; }


	friend constexpr bool operator<(const exStringView& lhs, const exStringView& rhs)noexcept {
		return lhs.compare_less(rhs);
	}
	friend constexpr bool operator>(const exStringView& lhs, const exStringView& rhs)noexcept {
		return rhs < lhs;
	}

	friend constexpr bool operator>=(const exStringView& lhs, const exStringView& rhs)noexcept {
		return !(lhs < rhs);
	}
	friend constexpr bool operator<=(const exStringView& lhs, const exStringView& rhs)noexcept {
		return !(rhs < lhs);
	}
	friend constexpr bool operator==(const exStringView& lhs, const exStringView& rhs)noexcept {
		return rhs.sz==lhs.sz && lhs.equal(rhs);
	}
	friend constexpr bool operator!=(const exStringView& lhs, const exStringView& rhs)noexcept {
		return !(lhs == rhs);
	}
	constexpr exStringView sub(size_type beg,size_type end)const noexcept {
		return{ data + beg,end - beg };
	}
	constexpr size_type find(const exStringView& src,size_type cur_pos=0)const noexcept {
		return cur_pos>=sz?
				npos:
				sub(cur_pos, cur_pos + src.sz) == src ? cur_pos : find(src, cur_pos + 1);
	}
	constexpr size_type find(char src,size_type cur_pos=0)const noexcept {
		return cur_pos>=sz?
				npos:
				data[cur_pos]==src? cur_pos: find(src,cur_pos+1);
	}
};

constexpr bool is_digit(char src)noexcept {
	return exStringView{ "0123456789" }.find(src)!=exStringView::npos;
}
constexpr bool is_alpha(char src)noexcept {
	return	exStringView{ 
				"abcdefghijklmnopqrstuvwxyz"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
			}.find(src)!=exStringView::npos;
}


/*
	\brief	用于构建一个纯编译期字符串类型
	\param	\R 所返回的字符容器,字符被保存在模板参数中
			\T 一个具有字符串成员str的类型
			\index... 用于选取str中字符的索引一般是 0,1,2 ... str.size()
*/
template<template<size_t...>class R,class T,size_t... index>
decltype(auto) buildStrType(IdSeq<index...>&&) {
	return R <T{}.str[index]...> {};
}

/*
	\brief	
	\param
	\return
	\note
*/
template<size_t N>
struct Formater {

};




