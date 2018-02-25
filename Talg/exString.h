#pragma once
#include <functional>
#include <algorithm>
#include "core.h"
#include "find_val.h"  //for no_index
#include <array>

namespace Talg{
template<class T>
constexpr T min(const T& a, const T& b) {
	return b < a ? b : a;
}
template<class T> 
constexpr T max(const T& a, const T& b)
{
	return a > b ? a : b;
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

}//namespace Talg





