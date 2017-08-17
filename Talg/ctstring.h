#pragma once
#include "core.h"
#include "index_seq.h"
#include <tuple>
#include "fcall.h"
#include "algorithm.h"
#include "type_traits.h"

namespace Talg{


template<class T,size_t N>
struct ctArray :ctArray<T, N - 1> {
	using Base = ctArray<T, N - 1>;
	static constexpr size_t length = N;
	T val;
	constexpr ctArray():Base(),val(){}
	constexpr ctArray(const ctArray& rhs) = default;

	template<size_t...Ids>
	constexpr ctArray(const ctArray<T, N-1>& lhs,const T& c, IdSeq<Ids...>)
		:Base(lhs[Ids+1]...,c), val(lhs.val){}

	constexpr Base to_base()const {
		return *this;
	}

	template<class U,class...Us>
	constexpr ctArray(const LimitTo<T,U>& first,const LimitTo<T,Us>&... args)
		:Base(forward_m(args)...),val(first){}

	constexpr T at(size_t id)const {
		return id == N-1 ? val : Base::at(id);
	}
	constexpr T operator[](size_t id)const {
		return id < N ? at(N - 1 - id) : throw std::runtime_error("index over range");
	}
	template<class U,size_t M>
	constexpr bool operator==(const ctArray<U,M>& rhs)const{
		return N!=M ?
			false:
			val == rhs.val && Base::operator==(static_cast<const ctArray<U, M - 1>&>(rhs));
	}
	template<class U,size_t M>
	constexpr bool operator!=(const ctArray<U,M>& rhs)const{
		return !operator==(rhs);
	}

};
template<class T>
struct ctArray<T, 1> {
	T val={};
	constexpr ctArray():val(){}
	constexpr ctArray(const T& v):val(v){}
	constexpr ctArray(T&& v):val(std::move(v)){}
	constexpr T at(size_t )const{
		return val;
	}
	constexpr T operator[](size_t )const{
		return val;
	}
	template<class U,size_t M>
	constexpr bool operator==(const ctArray<U,M>& rhs)const{
		return M != 1 ? false : val == rhs.val;
	}
	template<class U,size_t M>
	constexpr bool operator!=(const ctArray<U,M>& rhs)const{
		return M != 1 ? true : val != rhs.val;
	}
};


template<size_t N>
struct AppendCt {
	template<size_t M,class T>
	static constexpr auto call(const ctArray<T, M>& lhs, const ctArray<T, N>& rhs){
		return AppendCt<N-1>::call(
			ctArray<T, M + 1>(lhs, rhs.val,std::make_index_sequence<M-1>()),
			rhs.to_base()
		);
	}
};

template<>
struct AppendCt<1> {
	template<size_t M,class T>
	static constexpr auto call(const ctArray<T, M>& lhs, const ctArray<T, 1>& rhs){
		return ctArray<T, M + 1>(lhs, rhs.val,std::make_index_sequence<M-1>());
	}
};






template<size_t Len>
struct ctString {
	using size_type = std::size_t;
	static constexpr size_type npos = no_index;
	ctArray<char, Len> data;
	
	
	constexpr ctString(const ctString<Len>&args) = default;

	//template<size_t...Ls>
	//explicit constexpr ctString(const ctString<Ls>&...args)
	//:ctString(append(ctString<Ls>(args)...)){}
	
	template<size_t N>
	constexpr ctString(const char(&str)[N])
	:ctString(str,std::make_index_sequence<N>()){}
	constexpr char operator[](size_t index)const  {
		return data[index];
	}
	//template<class U>
	//constexpr bool stringEqual(size_t index,const U& )const noexcept{
	//	return true;//== rhs[index];// && (index == 0 ? true : stringEqual(index - 1, lhs, rhs));
	//}
	constexpr operator ctArray<char,Len>()const {
		return data;
	}
	constexpr bool operator==(ctArray<char,Len> rhs)const noexcept {
		return data == rhs;
	}

	constexpr auto size()const noexcept { return Len; }

	template<size_t L,size_t L2>
	friend constexpr ctString<L + L2-1> operator+(const ctString<L>& lhs, const ctString<L2>& rhs)noexcept;

	template<size_t L2>
	constexpr ctString<Len+L2-1> append(const ctString<L2>& rhs)const noexcept{
		return { data, rhs.data,std::make_index_sequence<Len-1>(),std::make_index_sequence<L2>() };
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
	:data(str[Index]...){}

	template<size_t L,size_t L2,size_t...Index,size_t...Index2>
	constexpr ctString(const ctArray<char,L>& str,const ctArray<char,L2> str2, 
		IdSeq<Index...>&&, IdSeq<Index2...>&&)
	:data(str[Index]...,str2[Index2]...){}


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
