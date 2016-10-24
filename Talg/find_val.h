#pragma once
#include "core.h"
#include "index_seq.h"

#include <limits>
#include <utility>

enum :size_t {
	no_index = std::numeric_limits<size_t>::max() - 1 //抑制VC的警告C4307
};
struct NoType :public Seq<NoType> {};

template<template<class...>class OpImp, class, class>
struct MultiopImp;

template<template<class...>class OpImp, class T, class...Ts, class...Us>
struct MultiopImp<OpImp, Seq<T, Ts...>, Seq<Us...>> {
	using type = Merge_s<
		OMIT_T(OpImp<
			T,
			Seq<Us...>
		>),
		typename MultiopImp<
			OpImp,
			Seq<Ts...>,
			Seq<Us...>
		>::type
	>;
	using value_type = std::conditional_t<
		OpImp<T, Seq<Us...>>::value == no_index,
		typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::value_type,
		MergeIndex< iTag< OpImp<T, Seq<Us...>>::value >,
			typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::value_type>
	>;
};

template<template<class...>class OpImp, class T, class...Us>
struct MultiopImp<OpImp, Seq<T>, Seq<Us...>> {
	using type = OMIT_T(OpImp<T, Seq<Us...>>);
	using value_type = std::conditional_t<
		OpImp<T, Seq<Us...>>::value == no_index,
		typename OpImp<T, Seq<Us...>>::type,//index_sequence<>,
		iTag<OpImp<T, Seq<Us...>>::value>
	>;
};

template<template<class...>class OpImp, class...Us>
struct MultiopImp<OpImp, Seq<>, Seq<Us...>> {
	using type = Seq<>;
	using value_type = index_sequence<>;
};


template<template<class...>class OpImp, class Var, class Konst>  //Konst means const
using Multiop_s = OMIT_T(MultiopImp<OpImp, assert_is_seq(Var), assert_is_seq(Konst)>);

template<template<class...>class OpImp, class Var, class Konst>
using Multiop = MultiopImp<OpImp, Seqfy<Var>, Seqfy<Konst>>;

template<template<class...>class OpImp, class Var, class Konst>
using Multiop_n = typename MultiopImp<OpImp, Seqfy<Var>, Seqfy<Konst>>::value_type;



template<size_t n, class Seq_res>
struct iSeqImp;

template< size_t n, class... Ts>
struct iSeqImp<n, Seq<Ts...>> {
	static constexpr size_t value = n;
	using type = Seq<Ts...>;
};

//iSeq只是多了个size_t的Seq罢了,可以通过value访问它
template<size_t n, class... Ts>
using iSeq = iSeqImp<n, Seq<Ts...>>;

//结果包装器,它可以自动进行递归计算(也即,+1或no_index),若计算结果为no_index则停止.
template<class T>
struct NextSeq {
	static constexpr size_t value = T::value != no_index ? T::value + 1 : no_index;
	using type = typename T::type;
};




template<class, class>struct FindImp;

template<class T, class...Ts>
struct FindImp<T, Seq<T, Ts...>> : iSeq<0, Ts...> {};

template<class T, class U, class...Ts>
struct FindImp<T, Seq<U, Ts...>> :NextSeq<FindImp<T, Seq<Ts...>>> {};

template<class T, class U>
struct FindImp<T, Seq<U>> :iSeq<no_index, NoType> {};

template<class T>
struct FindImp<T, Seq<T>> : iSeq<0> {};

template<class Dst, class Src>
using Find = FindImp<Dst, Seqfy<Src>>;
template<class Dst, class Src>
using Find_t = OMIT_T(FindImp<Dst, Seqfy<Src>>);
template<class Dst, class Src>
using Find_ss = OMIT_T(FindImp<Dst, assert_is_seq(Src)>);
template<class Dst, class Src>
using Find_svt = FindImp<Dst, assert_is_seq(Src)>;




template<class,class, class>struct Find_if_svt;

template<class Pred,class T, class U,class...Ts>
struct Find_if_svt<Pred,T, Seq<U, Ts...>> : 
	std::conditional_t< Pred::template pred<T,U>::value,
	iSeq<0, Ts...>,
	NextSeq< Find_if_svt<Pred,T,Seq<Ts...>>>
	>
{ };

template<class Pred,class T, class U>
struct Find_if_svt<Pred,T, Seq<U>> :
	std::conditional_t< Pred::template pred<T, U>::value,
	iSeq<0>,
	iSeq<no_index, NoType>
	>
{ };

template<class Pred,class Dst, class Src>
using Find_if_t = OMIT_T(Find_if_svt<Pred,Dst, Seqfy<Src>>);
template<class Pred,class Dst, class Src>
using Find_if_s = OMIT_T(Find_if_svt<Pred,Dst, assert_is_seq(Src)>);
//template<class Pred, class Dst, class Src>
//using Find_if = OMIT_T(Find_if_svt<Pred, Dst, Seqfy<Src>>);


