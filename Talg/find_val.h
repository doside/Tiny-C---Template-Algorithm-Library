#pragma once
#include "core.h"
#include "index_seq.h"
#include "seqop.h"
#include <limits>
#include <type_traits>
#include <utility>
#include "basic_macro_impl.h"

///todo: this file has too much complicated meta class,should reimplement those at all.


namespace Talg{
/*
todo fix 这个类过于特殊,没多少实际意义,似乎将index_type并入type中更为合理.
用于实现多次查找,第一个序列是所有要查找的东西,第二个序列是查找范围
在type中把当前查找的结果类型并入最终的结果类型,
在index_type中把当前的下标并入最终的下标序列.
在ForSeq中逐个挑出类型,分别查找它们在SrcSeq中的下标,谓词为OpImp
*/
template<template<class...>class OpImp, class ForSeq, class SrcSeq>
struct MultiopImp;

/*
	\brief	仅供MultiopImp用的内部类型,用于表示某个被删除的类型的占位符.
*/
struct DelParam{};

//todo: 当前的实现过于低效,采用查找(线性时间)然后替换(线性时间)然后再查找(线性时间)的方法来实现.
template<template<class...>class OpImp, class T, class...Ts, class...Us>
struct MultiopImp<OpImp, Seq<T, Ts...>, Seq<Us...>> {
	static constexpr size_t cur_index_ = OpImp<T, Seq<Us...>>::value;
	static_assert(cur_index_ != no_index, "fail to search the type of parameter.");
	static_assert(sizeof...(Ts) < sizeof...(Us),"src sequence can't be longer than dst sequence's length.");

	//此处的conditional是很有必要的,因为在no_index的情况下,
	//会导致编译器继续计算类型(尽管上面的断言会失败),最坏情况下导致gcc泄露了几GB内存然后崩溃
	
	using index_type = std::conditional_t<
		cur_index_ == no_index,
		void,
		MergeIndex< IdSeq< cur_index_ >,
			typename MultiopImp<OpImp, Seq<Ts...>, ReplaceAt<cur_index_,Seq<Us...>,DelParam>>::index_type>
	>;
	//typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::index_type,
	//using index_type = MergeIndex< IdSeq< cur_index_ >,
		//					typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::index_type>;
public:
	using type = Merge_s<
		omit_t_m(OpImp<T,Seq<Us...>>),
		typename MultiopImp<
			OpImp,
			Seq<Ts...>,
			ReplaceAt<cur_index_,Seq<Us...>,DelParam>
		>::type
	>;
};

template<template<class...>class OpImp, class T, class...Us>
struct MultiopImp<OpImp, Seq<T>, Seq<Us...>> {
	using type = omit_t_m(OpImp<T, Seq<Us...>>);
	using index_type = std::conditional_t<
		OpImp<T, Seq<Us...>>::value == no_index,
		typename OpImp<T, Seq<Us...>>::type,//IdSeq<>,
		IdSeq<OpImp<T, Seq<Us...>>::value>
	>;
};

template<template<class...>class OpImp, class...Us>
struct MultiopImp<OpImp, Seq<>, Seq<Us...>> {
	using type = Seq<>;
	using index_type = IdSeq<>;
};


template<template<class...>class OpImp, class Var, class Konst>  //Konst means const
using Multiop_s = omit_t_m(MultiopImp<OpImp, Var, Konst>);

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
	static constexpr size_t value = T::value != no_index ? T::value + (T::value != no_index) : no_index;
	//利用value +(T::value != no_index)来代替value + 1这样的写法从而防止编译器给出错误的溢出警告信息.
	using type = typename T::type;
};



/*
	\brief	在一个序列Src中寻找一个类型Target
	\tparam	\Target 要找的类型. \Src 一个类型序列Seq<...>,里面可能含有Target.
	\note	let 
				S=Seq<Ts...>  
				Res=Find_svt<T,S>
			than Merge_s<
				Before_s<Res::value,S>,
				Res::type
			> is same as Seq<Ts...>.
*/
template<class Target, class Src>struct Find_svt{};

/*!
	\brief	偏特化,当前的类型序列中的第一个即是要找的类型
	\tparam \T 要找的类型.
	\note	Seqi<0,Ts...>包含下标信息(要找的类型是在类型序列中是第几个类型)
			以及查找到的类型的后面还有哪些类型(Ts...),这可以用于继续接下来的其他操作.
*/
template<class T, class...Ts>
struct Find_svt<T, Seq<T, Ts...>> : Seqi<0, Ts...> {};

/*!
	\brief	偏特化,当前的类型序列中的第一个不是要找的类型
	\tparam \T 要找的类型.
	\note NextSeq会自动地把接下来的结果往上传递.
*/
template<class T, class U, class...Ts>
struct Find_svt<T, Seq<U, Ts...>> :NextSeq<Find_svt<T, Seq<Ts...>>> {};



/*!
	\brief	偏特化,当前的类型序列没有要找的类型
	\tparam \T 要找的类型.
	\note	no_index表示没有索引
*/
template<class T>
struct Find_svt<T, Seq<>> :Seqi<no_index> {};


/*!
	\brief	偏特化,要找的类型在当前的类型序列的最后.
	\tparam \T 要找的类型.
	\note	Seqi<0> 表示后续没有别的类型了,0表示索引,它会正确地往上传递并递增成正确的值.
*/
template<class T>
struct Find_svt<T, Seq<T>> : Seqi<0> {};

/*!
	\brief	相比于Find_svt增加了自动Seqfy的功能
	
			原本Find_svt要求被遍历的序列必须是Seq<...>,
			但是Find_vt消除了这个要求.
	\tparam \Dst要找的类型 \Src被遍历的类型序列
	\return 既有被查找的类型的索引,也有在其后的类型序列作为类型信息．
*/
template<class Dst, class Src>
using Find_vt = Find_svt<Dst, Seqfy<Src>>; 




/*!
	\brief	相比于Find_svt,直接返回类型,也即无法得知索引.
			避免写出typename Find_svt<...>::type
	\tparam \Dst要找的类型 \Src被遍历的类型序列
	\return Seq<...>
	\todo: 是否废弃该类型?
*/
template<class Dst, class Src>
using Find_ss = omit_t_m(Find_svt<Dst, Src>);

/*!
	\brief	相比于Find_ss增加了自动Seqfy的功能
			原本Find_ss要求被遍历的序列必须是Seq<...>,
			但是Find_t消除了这个要求.
	\tparam \Dst要找的类型 \Src被遍历的类型序列
	\return Seq<...>
	\todo: 是否废弃该类型?
*/
template<class Dst, class Src>
using Find_t = omit_t_m(Find_svt<Dst, Seqfy<Src>>);


/*!
	\brief	在类型序列中查找某个符合判定子的类型
			find type U in TypeSeq so that Pred<T,U>==true
	\tparam	\Pred 判定子,\TypeSeq类型序列.
	\return	Seqi<id,Ts...> 其中Ts...是找到的类型之后的所有类型,id是索引.
			Given TypeSeq == Seq<As...,U,Bs...>,
				find type U in TypeSeq so that PushBack<Pred,U> == true
				and return Seqi<sizeof...(As),Bs...>
*/
template<class Pred,class TypeSeq>struct FindIf_svt{};



/*!
	\brief	偏特化,判断第一个类型是否符合判定子
			specialized for predicate the first type in the type sequence.
	\tparam	\Pred	判定子	predicator,
			\TypeSeq	类型序列	type sequence.
	\return	Seqi<id,Ts...> 其中Ts...是找到的类型之后的所有类型,id是索引.
			Given TypeSeq == Seq<As...,U,Bs...>,
				find type U in TypeSeq so that PushBack<Pred,U> == true
				and return Seqi<sizeof...(As),Bs...>
*/
template<class Pred,class U,class...Ts>
struct FindIf_svt<Pred,Seq<U, Ts...>> : 
	std::conditional_t< 
		Pred::template pred<U>::value,
		Seqi<0, Ts...>,
		NextSeq< 
			FindIf_svt<Pred,Seq<Ts...>>
		>
	>
{ };

/*!
	\brief	偏特化用于终止递归以及保证 FindIf_svt<Pred, Seq<>>总是Seqi<no_index>
			specialized for stop the recurrence.
	\tparam	\Pred	判定子	predicator
	\return	Seqi<no_index>
	\note	no_index表示找不到
*/
template<class Pred>
struct FindIf_svt<Pred, Seq<>> : Seqi<no_index>{ };

template<class Pred,class SrcSeq>
using FindIf_t = omit_t_m(FindIf_svt<Pred,Seqfy<SrcSeq>>);
template<class Pred,class SrcSeq>
using FindIf_s = omit_t_m(FindIf_svt<Pred,SrcSeq>);

template<class Pred,class SrcSeq>
using FindIf_v = FindIf_svt<Pred,Seqfy<SrcSeq>>;


/*!
	\brief	给定一个模式类型Pattern,
			ConvertibleTo<Pattern>的pred模板可以判断某个目标类型是否能转换为模式类型.
			predicate a type Target whether is convertible to Pattern type.
	\tparam \Pattern 模式类型 the pattern type, 
*/
template<class Pattern>
struct ConvertibleTo{ 
	template<class Target>
	using pred = std::is_convertible<Target, Pattern>;
};



/*!
	\brief	给定一个模式类型,ConvertibleFrom类的pred模板可以
			判断某个目标类型是否可以从模式类型转换得来.
			Given a Pattern type,predicate a type Target 
			whether Pattern type could convert to Target
	\tparam \Pattern 模式类型 the pattern type, 
*/
template<class Pattern>
struct ConvertibleFrom { 
	template<class Target>
	using pred = std::is_convertible<Pattern, Target>;
};



/*!
	\brief	给定一个模式类型,SameAndConvertibleTo类的pred模板可以
			判断某个目标类型是否与模式类型相同且能转换为模式类型.
			Given a Pattern type,predicate a type Target 
				whether is the same as Pattern and whether is convertible to Pattern type.
	\tparam \Pattern 模式类型 the pattern type, 
*/
template<class Pattern>
struct SameAndConvertibleTo {
	template<class Target>
	using pred = 
		AndValue<
			std::is_same< std::decay_t<Target>, std::decay_t<Pattern> >,
			std::is_convertible<Target, Pattern>
		>;
};

template<class T,class S>
struct CountType_sv{};

template<class T,class...Ts>
struct CountType_sv<T,Seq<T,Ts...>>:Tagi<1+CountType_sv<T,Seq<Ts...>>::value>{};

template<class T,class U,class...Ts>
struct CountType_sv<T,Seq<U,Ts...>>:CountType_sv<T,Seq<Ts...>> {};

template<class T>
struct CountType_sv<T,Seq<>>:Tagi<0>{};

}//namespace Talg


#include "undef_macro.h"

