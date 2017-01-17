#pragma once
#include "core.h"
#include <utility>

using std::index_sequence;

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

//一个将数字绑定到类型的模板类
template<size_t n>
using iTag = index_sequence<n>;

template<class>struct IndexfyImp;

template<size_t...Ns>
struct IndexfyImp<Seq<index_sequence<Ns>>...> {
	using type = index_sequence<Ns...>;
};
template<class T>
using Indexfy = OMIT_T(IndexfyImp<T>);

template<class T,class U>
struct MergeIndexImp;

template<size_t...Ns,size_t...Ms>
struct MergeIndexImp<index_sequence<Ns...>, index_sequence<Ms...>> {
	using type = index_sequence<Ns..., Ms...>;
};


template<class T,class U>
using MergeIndex = OMIT_T(MergeIndexImp<T, U>);
template<class T, class U>
using MergeIndex_s = OMIT_T(MergeIndexImp<T, U>);