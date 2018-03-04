#pragma once

#include <utility>
#include <cstddef>
#include "basic_marco_impl.h"
/*
命名公约：
??表示零个或多个类型(例如,模板(template<class>class)不是类型,数字(size_t,int...)不是类型)

后缀不带_s表示对参数用了Seqfy,注意对 T 使用Seqfy可以保证是Seq<T>,对A<??>用则保证是Seq<??>
不带后缀时不对输出结果类型作任何假定 有时可能等同于_t,而有时则又可能需要使用omit_t_m

_s表示输入一定是Seq<??>,输出如果在语义上不可能存在返回多个类型的话那么结果就是 T, 否则结果是Seq<??>或Seq<T>
_n表示返回::index_type 一般是IdSeq<...>
_t表示返回::type
_v表示返回::value(现阶段由于msvc不支持std::??_v而没有使用这种命名)
_ss表示_s,此外还一定返回一个Seq<??>
_sv表示返回一个类型,里面具有::value可供使用
_st表示返回一个类型,里面具有::type 可供使用
_svt表示返回一个类型,里面具有::value和::type可供使用

struct后面没写public的那部分表示不推荐公有使用

*/

namespace Talg{



template< template<class...> class Dst >
struct TransformImp
{
	template< template<class...>class Src, class...Args >
	static Dst<Args...> from(Src<Args...>&&);

	template<class T, class...Args >
	static Dst<T> from(T&&);

};
template< template<class...> class Dst, class T>
using Transform = decltype(TransformImp<Dst>::from(std::declval<T>()));



template<class... Ps>
struct ReplaceParamImp
{
	template< template<class...>class Src, class...Args >
	static Src<Ps...>* from(Src<Args...>&&);//MSVC Workaround: 如果不使用指针的话,编译无法通过
	template<class T>
	static std::remove_pointer_t<T>* from(T&&);//MSVC Workaround: 如果不使用指针的话,编译无法通过
};



template<class T, class... Ps>
using ReplaceParam = std::remove_pointer_t<decltype(ReplaceParamImp<Ps...>::from(std::declval<T>()))>;



template<class T>
struct WrapperT {  //In some context ,we need a wrapper to use T which does't have ctor.
	using type = T;
};


/*
	\brief 用于在断言失败时视察某个类型具体是什么类型.
*/
template<class...T>
struct WithVal{
	static constexpr bool value = true;
	constexpr operator bool()const noexcept { return true; }
};



/*
	\brief	具有接受任意参数的构造子,并且不做任何事
	\param	anything
	\note	典型地,用于忽略函数调用的参数从而实现各种编译期元操作,参见@get
*/
struct EatParam {
	// \note 原本使用的不是模板,而是... 但Wnon-pod-varargs会使其无法通过编译
	template<class U>
	constexpr EatParam(U&&)noexcept {}
};
template<class>
using EatParam_t = EatParam;

template<class...Args>
struct Seq {
	constexpr Seq()noexcept{}
};
template<class T>
using Seqfy = Transform<Seq, T>;  //将 blabla<abcde>转换成Seq<abcde>

struct FailTrans{};

//Transform_t的实现细节.
template<
	template<class...>class Src,
	template<class...>class Dst,
	class...Ts,
	class...Us
>
Dst<Ts...> SeqTrans(Src<Ts...>&, Dst<Us...>&);

FailTrans SeqTrans(EatParam&&, EatParam&&);

/*
	\brief 把一个类型的参数列表变换成另一个类型的参数表
	\example Transform_t<AnySeq<int,float>,std::tuple<char,char,double>> == AnySeq<char,char,double>
	\param Dst被变化的目标类型,Src来源类型.
	\return Dst<???> 其中???与Src<???>的???一致
*/
template<class Dst,class Src>
using Transform_t = decltype(SeqTrans(std::declval<Src&>(), std::declval<Dst&>()));



template<class>struct SeqSize_sv;
template<class...Ts>
struct SeqSize_sv<Seq<Ts...>>:std::integral_constant<size_t , sizeof...(Ts)> {};
template<class T>
using SeqSize = SeqSize_sv<Seqfy<T>>;


template<class...Ts>
constexpr size_t countSeqSize(const Seq<Ts...>&)noexcept {
	return sizeof...(Ts);
}
template<class T>struct CountSeqSize:CountSeqSize<Seqfy<T>>{};
template<class...Ps>struct CountSeqSize<Seq<Ps...>>:std::integral_constant<std::size_t,sizeof...(Ps)>{};



template<class...>struct MergeImp;
template<class...Ts, class...Others>
struct MergeImp<Seq<Ts...>, Seq<Others...>>
{
	using type = Seq<Ts..., Others...>;
};
template<class first, class second>
using Merge = omit_t_m(MergeImp<Seqfy<first>, Seqfy<second> >);
template<class T, class U>
using Merge_s = omit_t_m(MergeImp<T, U>);


template<class...>struct ReverseImp;

template<class T, class...Ts>
struct ReverseImp<Seq<T, Ts...>> {
	using type = Merge_s<omit_t_m(ReverseImp<Seq<Ts...>>), Seq<T> >;
};
template<class T>
struct ReverseImp<Seq<T>> {
	using type = Seq<T>;
};

template<class T>
using Reverse = omit_t_m(ReverseImp<Seqfy<T>>);
template<class obj>
using Reverse_s = omit_t_m(ReverseImp<obj>);



template<bool expr,class...Ts>
constexpr bool staticCheck()noexcept{
	static_assert(expr, "see details of instantation information");
	return expr;
}
template<class T,class...Ts>
constexpr bool staticCheck()noexcept {
	//We provide this static_assert so that give client a chance to see more type information
	//and focus attention to this function,rather than other compile error noisy.
	//When you see this static_assert failed, you could check information of instantation
	//about this function in compiler log to see the more details,especially what T and Ts... are.
	//通过这个模板来携带一些必要的用于了解编译错误的类型信息,从而快速找到编译错误位点.
	//例如 简单的static_assert(is_same<T,U>::value,"")有时候是无法看见T U的具体类型信息的(比如TU是类模板参数)
	//而通过写static_assert(static_check<is_same<T,U>>(),"")则可以清楚地看到T U分别是什么.
	static_assert(T::value, "see details of instantation information");
	return T::value;
}

template<class T>
struct NotValue :std::integral_constant<bool, !T::value> { 
	constexpr NotValue()noexcept = default;
	constexpr operator bool()const noexcept { return this->value; }
};

template<class T,class...Ts>
struct AndValue: std::integral_constant< bool, T::value && AndValue<Ts...>::value > {
	constexpr AndValue()noexcept = default;
	constexpr operator bool()const noexcept {
		return this->value;
	}
};
template<class T>
struct AndValue<T>:std::integral_constant<bool, T::value>{
	constexpr AndValue()noexcept = default;
	constexpr operator bool()const noexcept {
		return this->value;
	}
};

template<class T,class...Ts>
struct OrValue: std::integral_constant< bool, T::value || OrValue<Ts...>::value > {
	constexpr OrValue()noexcept = default;
	constexpr operator bool()const noexcept {
		return this->value;
	}
};
template<class T>
struct OrValue<T>:std::integral_constant<bool, T::value>{
	constexpr OrValue()noexcept = default;
	constexpr operator bool()const noexcept {
		return this->value;
	}
};



}//namespace Talg
#include "undef_macro.h"
