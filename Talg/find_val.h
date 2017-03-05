#pragma once
#include "core.h"
#include "index_seq.h"

#include <limits>
#include <utility>





namespace Talg{
/*
todo fix 这个类过于特殊,没多少实际意义,似乎将index_type并入type中更为合理.
用于实现多次查找,第一个序列是所有要查找的东西,第二个序列是查找范围
在type中把当前查找的结果类型并入最终的结果类型,
在index_type中把当前的下标并入最终的下标序列.
*/
template<template<class...>class OpImp, class, class>
struct MultiopImp;

template<template<class...>class OpImp, class T, class...Ts, class...Us>
struct MultiopImp<OpImp, Seq<T, Ts...>, Seq<Us...>> {
	static_assert(OpImp<T, Seq<Us...>>::value != no_index, "fail to search the type of parameter.");
	static_assert(sizeof...(Ts) < sizeof...(Us),"src sequence can't be longer than dst sequence's length.");
	
	//此处的conditional是很有必要的,因为在no_index的情况下,
	//会导致编译器继续计算类型(尽管上面的断言会失败),最坏情况下导致gcc泄露了几GB内存然后崩溃
	using index_type = std::conditional_t<
		OpImp<T, Seq<Us...>>::value == no_index,
		void,
		MergeIndex< Tagi< OpImp<T, Seq<Us...>>::value >,
			typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::index_type>
	>;
	//typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::index_type,
	//using index_type = MergeIndex< Tagi< OpImp<T, Seq<Us...>>::value >,
		//					typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::index_type>;
public:
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
};

template<template<class...>class OpImp, class T, class...Us>
struct MultiopImp<OpImp, Seq<T>, Seq<Us...>> {
	using type = OMIT_T(OpImp<T, Seq<Us...>>);
	using index_type = std::conditional_t<
		OpImp<T, Seq<Us...>>::value == no_index,
		typename OpImp<T, Seq<Us...>>::type,//IdSeq<>,
		Tagi<OpImp<T, Seq<Us...>>::value>
	>;
};

template<template<class...>class OpImp, class...Us>
struct MultiopImp<OpImp, Seq<>, Seq<Us...>> {
	using type = Seq<>;
	using index_type = IdSeq<>;
};


template<template<class...>class OpImp, class Var, class Konst>  //Konst means const
using Multiop_s = OMIT_T(MultiopImp<OpImp, Var, Konst>);

template<template<class...>class OpImp, class Var, class Konst>
using Multiop = MultiopImp<OpImp, Seqfy<Var>, Seqfy<Konst>>;

template<template<class...>class OpImp, class Var, class Konst>
using Multiop_n = typename MultiopImp<OpImp, Seqfy<Var>, Seqfy<Konst>>::index_type;



template<size_t n, class Seq_res>
struct SeqiImp;

template< size_t n, class... Ts>
struct SeqiImp<n, Seq<Ts...>> {
	static constexpr size_t value = n;
	using type = Seq<Ts...>;
};

//Seqi只是多了个size_t的Seq罢了,可以通过value访问它
template<size_t n, class... Ts>
using Seqi = SeqiImp<n, Seq<Ts...>>;

//结果包装器,它可以自动进行递归计算(也即,+1或no_index),若计算结果为no_index则停止.
template<class T>
struct NextSeq {
	static constexpr size_t value = T::value != no_index ? T::value + 1 : no_index;
	using type = typename T::type;
};




template<class, class>struct Find_svt;

template<class T, class...Ts>
struct Find_svt<T, Seq<T, Ts...>> : Seqi<0, Ts...> {};

template<class T, class U, class...Ts>
struct Find_svt<T, Seq<U, Ts...>> :NextSeq<Find_svt<T, Seq<Ts...>>> {};

template<class T, class U>
struct Find_svt<T, Seq<U>> :Seqi<no_index, NoType> {};

template<class T>
struct Find_svt<T, Seq<T>> : Seqi<0> {};

template<class Dst, class Src>
using Find_vt = Find_svt<Dst, Seqfy<Src>>; 
template<class Dst, class Src>
using Find_t = OMIT_T(Find_svt<Dst, Seqfy<Src>>);
template<class Dst, class Src>
using Find_ss = OMIT_T(Find_svt<Dst, Src>);





template<class,class, class>struct Find_if_svt;

template<class Pred,class T, class U,class...Ts>
struct Find_if_svt<Pred,T, Seq<U, Ts...>> : 
	std::conditional_t< Pred::template pred<T,U>::value,
		Seqi<0, Ts...>,
		NextSeq< Find_if_svt<Pred,T,Seq<Ts...>>>
	>
{ };

template<class Pred,class T, class U>
struct Find_if_svt<Pred,T, Seq<U>> :
	std::conditional_t< Pred::template pred<T, U>::value,
		Seqi<0>,
		Seqi<no_index, NoType>
	>
{ };

template<class Pred,class Dst, class Src>
using Find_if_t = OMIT_T(Find_if_svt<Pred,Dst, Seqfy<Src>>);
template<class Pred,class Dst, class Src>
using Find_if_s = OMIT_T(Find_if_svt<Pred,Dst, Src>);

}//namespace Talg




