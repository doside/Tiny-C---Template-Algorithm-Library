#pragma once
#include <functional>
#include <algorithm>
#include "core.h"



template<class T>
constexpr T min(const T& a const T& b) {
	return b < a ? b : a;
}
template<class T> 
constexpr T max(const T& a, const T& b)
{
	return a > b ? a : b;
}


struct exString{
	const char* data;
	size_type sz;
	constexpr bool equal(const exString& rhs,size_type pos=0)noexcept {
		return  pos >= min(sz,rhs.sz) ? true : lhs[pos] == rhs[pos] && compare(rhs, pos + 1);
	}
	constexpr bool compare_less(const exString& rhs,size_type pos = 0)noexcept {
		return  pos >= sz ? 
					false : 
					data[pos] == rhs[pos]?
						compare_less(rhs,pos + 1):
						data[pos] < rhs[pos];
	}
public:
	using size_type = std::size_t;
public:
	template<size_type N>
	constexpr exString(const char (&str)[N]):data(str),sz(N-1){}
	constexpr exString(const char* str, size_type len):data(str),sz(len){}

	constexpr char operator[](size_type index)const noexcept { return data[index]; }
	constexpr size_type size()const noexcept { return sz; }


	friend constexpr bool operator<(const exString& lhs, const exString& rhs)noexcept {
		return lhs.compare_less(rhs);
	}
	friend constexpr bool operator>(const exString& lhs, const exString& rhs)noexcept {
		return rhs < lhs;
	}

	friend constexpr bool operator>=(const exString& lhs, const exString& rhs)noexcept {
		return !(lhs < rhs);
	}
	friend constexpr bool operator<=(const exString& lhs, const exString& rhs)noexcept {
		return !(rhs < lhs);
	}
	friend constexpr bool operator==(const exString& lhs, const exString& rhs)noexcept {
		return rhs.sz==lhs.sz && lhs.equal(rhs);
	}
	friend constexpr bool operator!=(const exString& lhs, const exString& rhs)noexcept {
		return !(lhs == rhs);
	}
	constexpr exString sub(size_type beg,size_type end)noexcept {
		return{ data + beg,end - beg };
	}
	constexpr size_type find(const exString& src,size_type cur_pos=0)noexcept {
		return sub(cur_pos, cur_pos + src.sz) == src ? cur_pos : find(src, cur_pos + 1);
	}
	
};
