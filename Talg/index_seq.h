#pragma once
#include "core.h"
#include <utility>

using std::size_t;

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