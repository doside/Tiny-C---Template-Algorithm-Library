#pragma once

#include <utility>
#include <cstddef>

/*
命名公约：
??表示零个或多个类型(例如,模板(template<class>class)不是类型,数字(size_t,int...)不是类型)

后缀不带_s表示对参数用了Seqfy,注意对 T 使用Seqfy可以保证是Seq<T>,对A<??>用则保证是Seq<??>
不带后缀时不对输出结果类型作任何假定 有时可能等同于_t,而有时则又可能需要使用OMIT_T

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




#define forward_m(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
#define except_when(...) noexcept(noexcept(__VA_ARGS__))
#define OMIT_T(...) typename __VA_ARGS__::type

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


template<class T>
struct WrapperT {  //In some context ,we need a wrapper to use T which does't have ctor.
	using type = T;
};
template<class...Args>
struct Seq { };
template<class T>
using Seqfy = Transform<Seq, T>;  //将 blabla<abcde>转换成Seq<abcde>


template<class>struct SeqSize_sv;
template<class...Ts>
struct SeqSize_sv<Seq<Ts...>>:std::integral_constant<size_t , sizeof...(Ts)> {};
template<class T>
using SeqSize = SeqSize_sv<Seqfy<T>>;


template<class...Ts>
constexpr size_t countSeqSize(const Seq<Ts...>&)noexcept {
	return sizeof...(Ts);
}



template<class...>struct MergeImp;
template<class...Ts, class...Others>
struct MergeImp<Seq<Ts...>, Seq<Others...>>
{
	using type = Seq<Ts..., Others...>;
};
template<class first, class second>
using Merge = OMIT_T(MergeImp<Seqfy<first>, Seqfy<second> >);
template<class T, class U>
using Merge_s = OMIT_T(MergeImp<T, U>);


template<class...>struct ReverseImp;

template<class T, class...Ts>
struct ReverseImp<Seq<T, Ts...>> {
	using type = Merge_s<OMIT_T(ReverseImp<Seq<Ts...>>), Seq<T> >;
};
template<class T>
struct ReverseImp<Seq<T>> {
	using type = Seq<T>;
};

template<class T>
using Reverse = OMIT_T(ReverseImp<Seqfy<T>>);
template<class obj>
using Reverse_s = OMIT_T(ReverseImp<obj>);



