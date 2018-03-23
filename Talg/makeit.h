#pragma once
#include <Talg/core.h>
#include <Talg/find_val.h>
#include <type_traits>

namespace Talg {

	/*
		\brief	占位用的tag type,表明该参数需要被推导.
		\see	\@makeit
	*/
	template<size_t N=1>
	struct NeedDeduce:std::integral_constant<size_t,N>{ };

	template<class...>
	struct MakeDeduce_st;

	/*!
		\brief	从待定类型中数出有多少待推导的类型,然后把前若干个参数的类型放进去.
		\param	S是待定类型,P是参数表的类型序列
		\return type 一个类型序列,其中包括了所有根据参数推导出的类型.
		\note	假定了S中所有的需要被推导的类型是连续出现在后方的.
				假定了P中的最前面几个参数的类型与被推导的类型一致.
	*/
	template<class P,class S>
	struct MakeDeduce_st<P,S> {
		enum :size_t {
			explicit_cnt = Find_if_svt<NeedDeduce, S>::value,
		};
		using type = Merge_s<
			std::conditional_t<explicit_cnt==no_index,S,Before_s<idx,S>>,
			Before_s<CountType_sv<NeedDeduce, S>::value, P>
		>;
	};

	/*
		\brief	
		\tparam	P为参数类型序列,S为被构造的类型 
		\return
		\note
	*/
	template<class P,class S>
	using MakeDeduce_t = typename MakeDeduce_st<Seqfy<P>,Seqfy<S>>::type;


	/*!
		\brief	给每个类都写一遍static make很烦,所以利用这个简(fu)单(za)的自动推导
				配合NeedDeduce使用,可以通过让被make的类在待推导的参数设置默认参数,
				或者是直接手动写NeedDeduce.
		\demo	makeit<A<T1,T2,NeedDeduce,NeedDeduce>>(1,2.0,'a')相当于
				A<T1,T2,int,double>(1,2.0,'a') 因为 1的类型为int,2.0的类型为double.
				多出来的参数(如,此例中的'a')则被无视掉.
				一般地,make<T<As...,NeedDeduce>>

		\param	args任意的可以forward的参数,只要T的构造子支持它们就好,
				注意有些类型是无法被forward的,如位段等.
		\return 一个对象T,由参数构造.
		\note	使用的是()的构造而非{},避免initializer_list的构造抢夺.
				假定了所有的需要被推导的类型是连续出现在后方的.
				假定了最前面几个参数的类型与被推导的类型一致.

		\todo	fix:当前不支持推导引用,这理论上是可以用NeedDeduce&&之类的实现的,
				但实现起来比较繁琐,暂时忽略.目前是直接decay之,也就是所有推导出来的类型都是decay过的.
	*/
	template<class T,class...Ts>
	static auto makeit(Ts&&...args) {
		return  Transform_t<T, MakeDeduce_t<Seq<std::decay_t<Ts>...>, T> >(forward_m(args)...);
	}
}


