﻿#ifndef SEQOP_H_INCLUDED
#define SEQOP_H_INCLUDED
#include "core.h"

namespace Talg{

template<class T, class U>struct Erase_frontImp;

template<class T>
struct Erase_frontImp<T, Seq<>> {
	using type = Seq<>;
};
template<class U, class T, class...Ts, class...Us>
struct Erase_frontImp< Seq<T, Ts...>, Seq<U, Us...> > {
	using type = Merge_s<Seq<U>, OMIT_T(Erase_frontImp<Seq<Ts...>, Seq<Us...>>)>;
};

template<class T, class...Ts, class...Us>
struct Erase_frontImp< Seq<T, Ts...>, Seq<T, Us...> > {
	using type = OMIT_T(Erase_frontImp<Seq<Ts...>, Seq<Us...>>);
};
template<class T, class...Us>
struct Erase_frontImp< Seq<T>, Seq<T, Us...> > {
	using type = Seq<Us...>;
};
template<class U, class T, class...Us>
struct Erase_frontImp< Seq<U>, Seq<T, Us...> > {
	using type = Seq<T, Us...>;
};

//Erase front<A(abc) B(abcdef)> is Seq<def>
//Erase font<A(abc) B(bacdef)> is Seq<badef>
template<class del, class from>
using Erase_front = OMIT_T(Erase_frontImp<Seqfy<del>, Seqfy<from>>);
template<class del, class from>
using Erase_front_s = OMIT_T(Erase_frontImp<del, from>);






//rotate(s1(abc),s2(abcdef)) is seq(defabc) 并不要求s1,s2是相同的元序列类型
template<class Obj, class Src>
using RotateLeft = Merge_s<Erase_front<Obj, Src>, Seqfy<Obj>>;
template<class Obj, class Src>
using RotateLeft_s = Merge_s<Erase_front_s<Obj, Src>, Obj>;





template<class >struct Pop_backImp;
template<class T, class...Ts>
struct Pop_backImp<Seq<T, Ts...>> {
	using type = Merge_s<Seq<T>, typename Pop_backImp<Ts...>::type>;
};
template<class T>
struct Pop_backImp<Seq<T>> {
	using type = Seq<>;
};
template<class T, class U>
struct Pop_backImp<Seq<T, U>> {
	using type = Seq<T>;
};
template<class T>
using Pop_back = OMIT_T(Pop_backImp<Seqfy<T>>);
template<class T>
using Pop_back_s = OMIT_T(Pop_backImp<T>);






template<class T>
struct HeadImp;
template<class T, class... Ts>
struct HeadImp<Seq<T, Ts...>> {
	using type = T;
};
template<class T>
struct HeadImp<Seq<T>> {
	using type = T;
};
template<class T>
using Head_s = OMIT_T(HeadImp<T>);
template<class T>
using Head = OMIT_T(HeadImp<Seqfy<T>>);



template<size_t, class...>struct BeforeImp;

template<size_t n, class T, class...Ts>
struct BeforeImp<n, Seq<T, Ts...>> {
	static_assert(n <= sizeof...(Ts)+1, "");
	using type = Merge_s<Seq<T>, OMIT_T(BeforeImp<n - 1, Seq<Ts...>>)>;
};

template<class T, class...Ts>
struct BeforeImp<1, Seq<T, Ts...>> {
	using type = Seq<T>;
};
template<class T, class...Ts>
struct BeforeImp<0, Seq<T, Ts...>> {
	using type = Seq<>;
};
template<size_t n>
struct BeforeImp<n, Seq<>> {
	using type = Seq<>;
};

template<size_t n, class T>
using Before = OMIT_T(BeforeImp<n, Seqfy<T>>);
template<size_t n, class T>
using Before_s = OMIT_T(BeforeImp<n, T>);



template<size_t, class...>struct AfterImp;
template<size_t n, class T, class...Ts>
struct AfterImp<n, Seq<T, Ts...>> {
	using type = OMIT_T(AfterImp<n - 1, Seq<Ts...>>);
};
template<class T, class... Ts>
struct AfterImp<0, Seq<T, Ts...>> {
	using type = Seq<Ts...>;
};
template<class T>
struct AfterImp<0, Seq<T>> {
	using type = Seq<>;
};
template<size_t n, class T>
struct AfterImp<n, Seq<T>> {
	using type = Seq<>;
};
template<size_t n>
struct AfterImp<n, Seq<>> {
	using type = Seq<>;
};

template<size_t n, class T>
using After = OMIT_T(AfterImp<n, Seqfy<T>>);
template<size_t n, class T>
using After_s = OMIT_T(AfterImp<n, T>);
//For any n,any Tuple:
//Merge_s< Before<n,Tuple<Ts...>>,
//         Seq<At<n,Tuple<Ts...>>>,
//          After<n,Tuple<Ts...>>  >  is Seq<Ts...>


template<class T, class U>struct Erase_backImp;
template<class... Ts, class... Us>
struct Erase_backImp<Seq<Ts...>, Seq<Us...>> {
	using type =
		Merge_s<
		Before_s<sizeof...(Us)-sizeof...(Ts), Seq<Us...>>,
		Erase_front_s<Seq<Ts...>, After_s<sizeof...(Us)-sizeof...(Ts)-1, Seq<Us...>>>
		>;

};

template<class T,class U>
using Erase_back_s = OMIT_T(Erase_backImp<T,U>);
template<class T, class U>
using Erase_back = OMIT_T(Erase_backImp<T, Seqfy<U>>);

//eg: RotateRight<Seq1<C,D>,Seq2<A,B,C,D>> is Seq<C,D, A,B>
template<class Obj, class Src>
using RotateRight = Merge_s<Seqfy<Obj>, Erase_back<Obj, Src>>;
template<class Obj, class Src>
using RotateRight_s = Merge_s<Obj, Erase_back_s<Obj, Src>>;


//用于高效实现get<T>(args...)以及apply<EndTagType>(func,args...)的内部类
template<class...>struct type_atImp;

template<class T,class...Ts>
struct type_atImp<T, Seq<Ts...>> {
	using type = OMIT_T(type_atImp<T, Seq<>, Seq<Ts...>>);
};
template<class T,class...Ts,class...Us>
struct type_atImp<T, Seq<Ts...>, Seq<T, Us...>> {
	using type = Seq<Ts...>;
};

template<class T, class U, class...Ts>
struct type_atImp<T, Seq<Ts...>, Seq<U>> {
	using type = Seq<Ts...,U>;
};
template<class T,class...Ts>
struct type_atImp<T, Seq<Ts...>, Seq<T>> {
	using type = Seq<Ts...>;
};
template<class T, class U, class...Ts, class...Us>
struct type_atImp<T, Seq<Ts...>, Seq<U, Us...>> {
	using type = OMIT_T(type_atImp<T, Seq<Ts...,U>, Seq<Us...>>);
};


/*
	\brief	产生n个T组成的序列
	\param	n表示T的个数
	\return	n==0时产生Seq<>,n==1时产生Seq<T>,n==2时产生Seq<T,T> 类推.
	\note	采用了二分实现来减少实例化深度.
*/
template<size_t n,class T>
struct GenerateSeqImp {
	using type = Merge_s<OMIT_T(GenerateSeqImp<n / 2,T>), OMIT_T(GenerateSeqImp<n - n / 2,T>)>;
};
template<class T>
struct GenerateSeqImp<0,T> {
	using type = Seq<>;
};
template<class T>
struct GenerateSeqImp<1,T> {
	using type = Seq<T>;
};
template<size_t N,class T>
using GeneratorSeq_t = OMIT_T(GenerateSeqImp<N, T>);


template<size_t Index,class T>
using EraseAt_s = Merge_s<Before_s<Index, T>, After_s<Index, T>>;


template<size_t Index,class T,class NewT>
using ReplaceAt = Merge_s<
	Merge<Before_s<Index, T>, NewT>,
	After_s<Index, T>
>;





}//namespace Talg


#endif //SEQOP_H_INCLUDED

