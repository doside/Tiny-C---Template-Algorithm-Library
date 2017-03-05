﻿#pragma once
#include "core.h"
#include <utility>
#include <limits>
//using std::size_t;
namespace Talg{
enum :size_t {
	no_index = std::numeric_limits<size_t>::max() - 1 //抑制VC的警告C4307
};
struct NoType :public Seq<NoType> {};

template<size_t... nums>
using IdSeq = std::index_sequence<nums...>;
/*
如果自己实现一个IdSeq的话,还要写个makeIdSeq<N>(),
这种设施的高效实现比较复杂,并且有可能是直接由编译器内建从而加快编译速度的,
此外如果两者不同的话, 用户混用了index_sequence和IdSeq会使得产生不必要的实例化
干脆还是用std算了
template<size_t... nums>
struct IdSeq {
	using value_type = size_t;

	int no_used=0;
	//constexpr IdSeq(int){}
	//constexpr IdSeq(){}
	constexpr IdSeq(int ){}
	constexpr IdSeq(){}
	static constexpr std::size_t size() {
		return sizeof...(nums);
	}
};
*/

#if 0
todo plan idea
struct IndexTag
{
	size_t value;
	constexpr IndexTag(size_t val)noexcept: value{val}{ }
	constexpr operator size_t()noexcept {
		return value;
	}
};
#endif

/*
	\brief	一个将数字绑定到类型的模板类
*/
template<size_t n>
using Tagi = IdSeq<n>;

template<class>struct IndexfyImp;

template<size_t...Ns>
struct IndexfyImp<Seq<IdSeq<Ns>>...> {
	using type = IdSeq<Ns...>;
};
template<class T>
using Indexfy = OMIT_T(IndexfyImp<T>);

template<class T,class U>
struct MergeIndexImp;

template<size_t...Ns,size_t...Ms>
struct MergeIndexImp<IdSeq<Ns...>, IdSeq<Ms...>> {
	using type = IdSeq<Ns..., Ms...>;
};


template<class T,class U>
using MergeIndex = OMIT_T(MergeIndexImp<T, U>);
template<class T, class U>
using MergeIndex_s = OMIT_T(MergeIndexImp<T, U>);

template<size_t...Ns>
Seq<Tagi<Ns>...> makeTagiSeq(IdSeq<Ns...>&&);

template<size_t N>
using TagiSeqN = decltype(makeTagiSeq(std::make_index_sequence<N>()));


}//namespace Talg
