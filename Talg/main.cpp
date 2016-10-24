


#include <tuple>
#include <string>
#include <vector>
#include <chrono>
#include <boost/type_index.hpp>

#include <type_traits>
#include <utility>
#include <iostream>
#include <limits>
using std::size_t;
#define forward_m(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)


#include <functional>
#include <iostream>


//这里没有使用单个参数而是使用变参宏,因为对单参数的宏传递多个参数是ill-form
#define OMIT_T(...) typename __VA_ARGS__::type



template<class...Args>
class Seq_ {};
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





template<class...Args>
struct Seq {

};
template<class T>
using Seqfy = Transform<Seq, T>;  //将 blabla<abcde>转换成Seq<abcde>


template<class T>struct SeqSize_s;

template<class...Ts>
struct SeqSize_s<Seq<Ts...>> :std::integral_constant<size_t, sizeof...(Ts)> {};

template<class T>
using SeqSize = SeqSize_s<Seqfy<T>>;

#if 0
template<size_t...>
struct IndexSeq {};

template< template<size_t...> class Dst >
struct iTransformImp
{
	template< template<size_t...>class Src, size_t...Args >
	static Dst<Args...> from(Src<Args...>&&);

	template<size_t n>
	static Dst<n> from(IndexSeq<n>&&);

};

template< template<size_t...> class Dst, class T>
using iTransform = decltype(TransformImp_i<Dst>::from(std::declval<T>()));
template<class T>
using iSeqfy = Transform<IndexSeq, T>;

template<class T>struct IndexSeqfyImp;

template<template<size_t...> class Set, size_t...Ns>
struct IndexSeqfyImp<Set<Ns...>>
{
	using type = Seq< IndexSeq<Ns>...>;
};

template<class T>
using IndexSeqfy = OMIT_T(IndexSeqfyImp<iSeqfy<T>>);

#endif

#ifdef TALG_LESS_CHECK
template<class T>
struct AssertIsSeqImp;
template<class... Ts>
struct AssertIsSeqImp<Seq<Ts...>> {
using type = Seq<Ts...>;
};

#define assert_is_seq(...) OMIT_T(AssertIsSeqImp<__VA_ARGS__>)
#else
#define assert_is_seq(...) __VA_ARGS__
#endif // TALG_LESS_CHECK




//输入是 多种???<....> 输出是Seq<....>
//Multitypelist Operation (Mop)


//Merge<A(abc) B(def)> is Seq<abcdef>
template<class...>struct MergeImp;
template<class...Ts, class...Others>
struct MergeImp<Seq<Ts...>, Seq<Others...>>
{
using type = Seq<Ts..., Others...>;
};
template<class first, class second>
using Merge = OMIT_T(MergeImp<Seqfy<first>, Seqfy<second> >);
template<class T, class U>
using Merge_s = OMIT_T(MergeImp<assert_is_seq(T), assert_is_seq(U)>);


#define declareTemplass(name) template<class...>struct name##Imp

declareTemplass(Reverse);
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
using Reverse_s = OMIT_T(ReverseImp<assert_is_seq(obj)>);




//Erase_front<A(abc) B(abcdef)> is Seq<def>
template<class T, class U>struct Erase_frontImp;
template<class T, class...Ts, class...Us>
struct Erase_frontImp< Seq<T, Ts...>, Seq<T, Us...> > {
using type = OMIT_T(Erase_frontImp<Seq<Ts...>, Seq<Us...>>);
};
template<class T, class...Us>
struct Erase_frontImp< Seq<T>, Seq<T, Us...> > {
using type = Seq<Us...>;
};
template<class del, class from>
using Erase_front = OMIT_T(Erase_frontImp<Seqfy<del>, Seqfy<from>>);   //Erase_frontImp<seqfy(del),seqfy(from)>
template<class del, class from>
using Erase_front_s = OMIT_T(Erase_frontImp<assert_is_seq(del), assert_is_seq(from)>);



#if 0
todo:
template<class...>struct Erase_backImp;
template<class...Ts, class...Us>
struct Erase_backImp<Seq<Ts...>, Seq<Ts..., Us...>> {
	using type = Seq<Ts...>;
};
//Erase_back<A(def), B(abcdef)> is Seq<abc>
template<class del, class from>
using Erase_back = OMIT_T(Erase_backImp<Seqfy<del>, Seqfy<from> >);
template<class del, class from>
using Erase_back_s = OMIT_T(Erase_backImp<assert_is_seq(del), assert_is_seq(from)>);
#endif // 0

template<class del, class from>
using Erase_back_s = Reverse< Erase_front_s<Reverse_s<del>, Reverse_s<from>> >;
template<class del, class from>
using Erase_back = Erase_back_s<Seqfy<del>, Seqfy<from>>;


//rotate(s1(abc),s2(abcdef)) is seq(defabc) 并不要求s1,s2是相同的元序列类型
template<class Obj, class Src>
using RotateLeft = Merge_s<Erase_front<Obj, Src>, Seqfy<Obj>>;
template<class Obj, class Src>
using RotateLeft_s = Merge_s<Erase_front_s<Obj, Src>, Obj>;


//eg: RotateRight<Seq1<C,D>,Seq2<A,B,C,D>> is Seq<C,D, A,B>
template<class Obj, class Src>
using RotateRight = Merge_s<Seqfy<Obj>, Erase_back<Obj, Src>>;
template<class Obj, class Src>
using RotateRight_s = Merge_s<Obj, Erase_back_s<Obj, Src>>;



//所有的输入都是同类型元序列 输出Seq<...>
//e.g. : op(A<..> , A<...>)   -------->  Seq<.....>
//SametypeList Operation





template<size_t n, class T>
struct AtImp;
template<size_t n, class T, class... Ts>
struct AtImp<n, Seq<T, Ts...>>
{
	static_assert(n <= sizeof...(Ts), "");
	using type = OMIT_T(AtImp<n - 1, Seq<Ts...>>);
};
template<class T, class... Ts>
struct AtImp<0, Seq<T, Ts...>>
{
	using type = T;
};
template<class T>
struct AtImp<0, Seq<T>>
{
	using type = T;
};
template<size_t n, class T>
using At = OMIT_T(AtImp<n, Seqfy<T>>);
template<size_t n, class T>
using At_s = OMIT_T(AtImp<n, assert_is_seq(T)>);


template<size_t n>
struct SelectImp {
	template<class T, class...Ts>
	static decltype(auto) get(T&&, Ts&&...args)noexcept {
		return SelectImp<n - 1>::get(forward_m(args)...);
	}
};
template<>
struct SelectImp<0> {
	template<class T>
	static decltype(auto) get(T&& arg)noexcept {
		return forward_m(arg);
	}
	template<class T, class... Ts>
	static decltype(auto) get(T&& arg, Ts&&...)noexcept {
		return forward_m(arg);
	}
};

template<size_t n, class... Ts>
decltype(auto) select(Ts&&... args)noexcept {
	return SelectImp<n>::get(forward_m(args)...);
}


/*
Seq<
op<Var1,Konst>
op<Var2,Konst>
op<..., Konst>
>
*/
/*
struct NoType;
template<class T>
struct iSeqImp{
template<T,class>
struct SeqImp;

template<T v1,T...vs>
struct SeqImp<v1,std::integer_sequence<T,vs...>>{
using type=std::integer_sequence<T,v1,vs...>;
};
template<T v1>
struct SeqImp<v1,NoType>{
using type=std::integer_sequence<T,v1>;
};
};
*/

struct NoType :public Seq<NoType> {};
template<size_t, class>
struct intSeqImp;

template<size_t v1, size_t...vs>
struct intSeqImp<v1, std::index_sequence<vs...>> {
	using type = std::index_sequence<v1, vs...>;
};

template<size_t v1>
struct intSeqImp<v1, NoType> {
	using type = std::index_sequence<v1>;
};

template<size_t vs, class T>
using intSeq = OMIT_T(intSeqImp<vs, T>);

enum :size_t {
	no_index = std::numeric_limits<size_t>::max() - 1 //抑制VC的警告C4307
};

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
		intSeq< OpImp<T, Seq<Us...>>::value,
		typename MultiopImp<OpImp, Seq<Ts...>, Seq<Us...>>::value_type>
	>;
};

template<template<class...>class OpImp, class T, class...Us>
struct MultiopImp<OpImp, Seq<T>, Seq<Us...>> {
	using type = OMIT_T(OpImp<T, Seq<Us...>>);
	using value_type =
		std::conditional_t<
		OpImp<T, Seq<Us...>>::value == no_index,
		NoType,
		std::index_sequence<OpImp<T, Seq<Us...>>::value>
		>;
};
template<template<class...>class OpImp, class Var, class Konst>  //Konst means const
using Multiop_s = MultiopImp<OpImp, assert_is_seq(Var), assert_is_seq(Konst)>;

template<template<class...>class OpImp, class Var, class Konst>
using Multiop = OMIT_T(MultiopImp<OpImp, Seqfy<Var>, Seqfy<Konst>>);

template<template<class...>class OpImp, class Var, class Konst>
using Multiop_n = typename MultiopImp<OpImp, Seqfy<Var>, Seqfy<Konst>>::value_type;



template<size_t n, class Seq_res>
struct iSeqImp;

template< size_t n, class... Ts>
struct iSeqImp<n, Seq<Ts...>> {
	static constexpr size_t value = n;
	using type = Seq<Ts...>;
};

template<size_t n, class... Ts>
using iSeq = iSeqImp<n, Seq<Ts...>>;


template<class T>
struct NextSeq {
	template<class U>
	using omit = typename U::type;

	static constexpr size_t value = T::value != no_index ? T::value + 1 : no_index;
	using type = typename T::type;
};




template<class, class>struct Find;

template<class T, class...Ts>
struct Find<T, Seq<T, Ts...>> : iSeq<0, Ts...> {};

template<class T, class U, class...Ts>
struct Find<T, Seq<U, Ts...>> :NextSeq<Find<T, Seq<Ts...>>> {};

template<class T, class U>
struct Find<T, Seq<U>> :iSeq<no_index, NoType> {};

template<class T>
struct Find<T, Seq<T>> : iSeq<0> {};

template<class Dst, class Src>
using Find_t = OMIT_T(Find<Dst, Seqfy<Src>>);
template<class Dst, class Src>
using Find_s = OMIT_T(Find<Dst, assert_is_seq(Src)>);

template<class...>struct Pop_backImp;
template<class T, class...Ts>
struct Pop_backImp<T, Ts...> {
	using type = Merge_s<Seq<T>, typename Pop_backImp<Ts...>::type>;
};
template<class T>
struct Pop_backImp<T> {
	using type = Seq<>;
};
template<class T, class U>
struct Pop_backImp<T, U> {
	using type = Seq<T>;
};
template<class...Ts>
using Pop_back = OMIT_T(Pop_backImp<Ts...>);



template<class...Ts>struct LongFirstCall;

template<class...Ts>
struct LongFirstCall<Seq<Ts...>> :public LongFirstCall<Pop_back<Ts...>> {
	using LongFirstCall<Pop_back<Ts...>>::pcall;
	using self = LongFirstCall<Seq<Ts...>>;

	//Us&&...的目的是让所有pcall都能以同样的方式调用
	//self* 是为了利用派生类到基类的转换顺序来逐个sfinae掉不合法的调用
	template<class F, class...Us>
	auto pcall(self*, F&& f, Ts&&...args, Us&&...)
		->Seq< decltype(forward_m(f)(forward_m(args)...)), Ts...>;
	//返回的类型为 Seq<Seq<这个重载所使用的参数类型>,f返回值的类型>
	//若这调用不合法那么一定会Substitution Failure，然后把指针的类型自动转换为基类指针


	//fix: 若调用不是Substitution Failure而是Ambiguous的话,是否可以继续转换成基类指针再判定？
	//todo: fix: vs2015 update2之前是无法在decltype内部使用pack expansion 的
};
template<>
struct LongFirstCall<Seq<>> {
	using self = LongFirstCall<Seq<>>;
	template<class F, class...Us>
	auto pcall(self*, F&& f, Us&&...)
		//->decltype(f());
		->Seq<>;
};

template<class T>
struct LongFirstCall<Seq<T>> :LongFirstCall<Seq<>> {
	using LongFirstCall<Seq<>>::pcall;
	using self = LongFirstCall<Seq<T>>;

	template<class F, class...Us>
	auto pcall(self*, F&& f, T&&arg, Us&&...)
		//->decltype(f(arg));
		->Seq<decltype(forward_m(f)(forward_m(arg))), T>;
};

struct LongParser
{
	template<class F, class...Ts>
	auto operator()(F&& f, Ts&&...args)
	-> decltype(LongFirstCall<Seq<Ts...>>{}
	.pcall(
		(LongFirstCall<Seq<Ts...>>*)nullptr,
		forward_m(f), forward_m(args)...
	));

	template<class F>
	Seq<> operator()(F&& f...);
};




template<class...Ts>struct ShortFirstCall;

template<class...Ts, class U, class...Us>
struct ShortFirstCall<Seq<Ts...>, U, Us...>
	:ShortFirstCall< Seq<Ts..., U>, Us...>
{
	using ShortFirstCall< Seq<Ts..., U>, Us...>::pcall;
	using self = ShortFirstCall<Seq<Ts...>, U, Us...>;

	template<class F, class...Ps>
	auto pcall(self*, F&& f, Ts&&...args, Ps&&...)
		->Seq<decltype(forward_m(f)(forward_m(args)...)), Ts...>;
};

template<class...Ts>
struct ShortFirstCall<Seq<Ts...>> {
	using self = ShortFirstCall<Seq<Ts...>>;

	template<class F, class...Ps>
	auto pcall(self*, F&& f, Ts&&...args, Ps&&...)
		->Seq<decltype(forward_m(f)(forward_m(args)...)), Ts...>;
};

struct ShortParser
{
	template<class F, class First, class...Ts>
	auto operator()(F&& f, First&& arg0, Ts&&...args)
		-> decltype(
			ShortFirstCall<Seq<First>, Ts...>{}.pcall(
			(ShortFirstCall<Seq<First>, Ts...>*)nullptr,
				forward_m(f), forward_m(arg0), forward_m(args)...
			));
	template<class F>
	Seq<> operator()(F&&...);
};


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
using Before_s = OMIT_T(BeforeImp<n, assert_is_seq(T)>);



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
using After_s = OMIT_T(AfterImp<n, assert_is_seq(T)>);
//For any n,any Tuple:
//Merge_s< Before<n,Tuple<Ts...>>,
//         Seq<At<n,Tuple<Ts...>>>,
//          After<n,Tuple<Ts...>>  >  is Seq<Ts...>



template<class...Ts>
struct GetImp {
	template<class T, class...Us>
	static decltype(auto) impl(Ts&&..., T&& obj, Us&&...) {
		return std::forward<T>(obj);
	}

	/*
	template<class...Us>
	static auto subSeq(Ts&&...args, Us&&...) {
	return std::forward_as_tuple(forward_m(args)...);
	}*/
	template<class F, class...Us>
	static decltype(auto) applyFrontImp(F&& func, Ts&&...args, Us&&...) {
		return forward_m(func)(forward_m(args)...);
	}
	template<class F, class...Us>
	static decltype(auto) applyBackImp(F&& func, Ts&&..., Us&&...args) {
		return forward_m(func)(forward_m(args)...);
	}
};

template<size_t n, class...Ts>
decltype(auto) get(Ts&&...args) {
	static_assert(n<sizeof...(args), "");
	return Transform<GetImp, Before_s<n, Seq<Ts...>> >
		::impl(forward_m(args)...);
}

template<size_t n, class F, class...Ts>
decltype(auto) applyFront(F&& func, Ts&&... args) {
	return Transform<GetImp, Before_s<n, Seq<Ts...>> >
		::applyFrontImp(forward_m(func), forward_m(args)...);
}
template<size_t n, class F, class...Ts>
decltype(auto) applyBack(F&& func, Ts&&... args) {
	return Transform<GetImp, Before_s<n, Seq<Ts...>> >
		::applyBackImp(forward_m(func), forward_m(args)...);
}

template<class...Ts>
struct ApplyRagImp {
	template<size_t end, class F, class...Us>
	static decltype(auto) call(std::index_sequence<end>, F&& func, Ts&&..., Us&&...args) {
		return applyFront<end>(forward_m(func), forward_m(args)...);
	}
};

template<size_t beg, size_t end, class F, class...Ts>
decltype(auto) applyRag(F&& func, Ts&&... args) {
	return Transform<ApplyRagImp, Before_s<beg, Seq<Ts...>> >
		::call(std::index_sequence<end - beg >{}, forward_m(func), forward_m(args)...);
}
template<size_t... Indices, class F, class...Ts>
decltype(auto) apply(std::index_sequence<Indices...>, F&& func, Ts&&... args) {
	return forward_m(func)(get<Indices>(forward_m(args)...)...);  //O(n^2)
}


template<class F, class...Ts>
void repeatAny(F&& func, Ts&&...args);


template<class...Ts>
struct GetSeqImp {
	template<class F>
	static decltype(auto) repeat(F&& func, Ts&&...args) {
		return forward_m(func)(forward_m(args)...);
	}
	template<class F, class...Us>
	static decltype(auto) repeat(F&& func, Ts&&...args, Us&&...last) {
		forward_m(func)(forward_m(args)...);
		return Transform< ::GetSeqImp, Before_s<sizeof...(Ts), Seq<Us...>> >
			::repeat(forward_m(func), forward_m(last)...);
	}

	template<class F, class...Us>
	static void any(F&& func, Ts&&...args, Us&&...last) {
		forward_m(func)(forward_m(args)...);
		repeatAny(forward_m(func), forward_m(last)...);
	}
	template<class F, class...Us>
	static void any(F&& func, Ts&&...args) {
		forward_m(func)(forward_m(args)...);
	}

	template<class ResCombiner, class F, class...Us>
	static decltype(auto) repeatSave(ResCombiner&& res, F&& func, Ts&&...args, Us&&...last) {
		//call ResCombiner::operator() to save res.
		forward_m(res) ((func)(forward_m(args)...));
		return forward_m(res) (Transform< ::GetSeqImp, Before_s<sizeof...(Ts), Seq<Us...>> >
			::repeat(forward_m(func), forward_m(last)...));
	}

};


template<size_t n, class F, class...Ts>
decltype(auto) repeat(F&& func, Ts&&... args) {
	using begin = Before_s<n, Seq<Ts...>>;
	static_assert(sizeof...(args) % n == 0, "m args should be divide into m/n group");
	return Transform<GetSeqImp, begin>::repeat(forward_m(func), forward_m(args)...);
}

template<class F, class...Ts>
void repeatAny(F&& func, Ts&&...args) {
	using param_parser = decltype(ShortParser{}(forward_m(func), forward_m(args)...));
	constexpr size_t param_length = SeqSize<param_parser>::value - 1;
	using begin = Before_s<param_length, Seq<Ts...>>;

	return Transform<GetSeqImp, begin>::any(forward_m(func), forward_m(args)...);
}


struct Printer1 {
	unsigned inner = 0;
	void operator()(int val) {
		std::cout << "Print1::operator()(int):   " << val << std::endl;
	}
	void operator()(double val) {
		std::cout << "Print1::operator()(double):   " << val << std::endl;
	}
	template<class...Ts>
	void operator()(Ts&&...args) {
		repeat<1>(
			[this](auto&& val) {
			++inner;
			std::cout << "Print1::operator()(Ts...):   " << val << std::endl;
		},
			forward_m(args)...
			);
		std::cout << "inner repeat: " << inner << std::endl;
		inner = 0;
	}
};
struct Printer2 {
	void operator()(int val) {
		std::cout << "Print2::operator()(int):   " << val << std::endl;
	}
	void operator()(int val)const {
		std::cout << "Print2::operator()(int)const:   " << val << std::endl;
	}
	void operator()(double val)const {
		std::cout << "Print2::operator()(double)const:   " << val << std::endl;
	}
	void operator()(double val) {
		std::cout << "Print2::operator()(double):   " << val << std::endl;
	}
	template<class...Ts>
	void operator()(Ts&&...args) {
		repeat<1>(
			[](auto&& val) {
			std::cout << "Print2::operator()(Ts...):   " << val << std::endl;
		},
			forward_m(args)...
			);
	}
	template<class...Ts>
	void operator()(Ts&&...args)const {
		repeat<1>(
			[](auto&& val) {
			std::cout << "Print2::operator()(Ts...)const:   " << val << std::endl;
		},
			forward_m(args)...
			);
	}
};
#if 0
template<class>struct IndexfyImp;
template<size_t...Ns>
struct IndexfyImp<Seq<IndexSeq<Ns>>...> {
	using type = IndexSeq<Ns...>;
};
template<class T>
using Indexfy = OMIT_T(IndexfyImp<T>);

template<size_t beg,size_t end>
struct BuildRagImp
{
	using seq_type = After_s<beg - 1, IndexSeqfy<std::make_index_sequence<end - 1>>>;
	using type = Indexfy<seq_type>;
};
template<size_t beg,size_t end>
using BuildRag = OMIT_T(BuildRagImp<beg, end>);



template<class P,class F,class...Ts>
decltype(auto) tryInvoke(P&& parser, F&& func,Ts&&...) {
	auto parse_func=[&parser, &func](auto&&...args) {
		forward_m(parser)(forward_m(func), forward_m(args)...);
	};
	return applyFront<n + 1>(parse_func, forward_m(args)...);
}

template<size_t n>
struct CompleteParser {
	template<class T>
	using fail = std::is_same<Seq<>, T>;
	template<class... Ts>
	using cond = std::conditional_t<Ts...>;

	template<class F,class...Ts>
	auto impl(F&& func, Ts&&...args)
	->cond<fail<decltype(applyFront<n>(forward_m(func),forward_m(args)...))>::value,
		decltype(CompleteParser<n + 1>{}.impl(forward_m(args)...)),
		
		cond<fail<decltype(applyBack<n>(CompleteParser<0>{},forward_m(args)...))>::value,
			decltype(CompleteParser<n + 1>{}.impl(forward_m(args)...)),
			decltype(applyBack<n>(CompleteParser<0>{}, forward_m(args)...)),
		>
	  >

};

#endif




#define COUNT_RES(...) At<0,decltype(LongParser{}(__VA_ARGS__))>

#define Is_Same(...) \
AssertIsSame<__VA_ARGS__>{}

template<class L, class R>
struct AssertIsSame {
	static_assert(std::is_same<L, R>::value, "");
};

struct Foo {
	char operator()(int);
	int operator()(double, int);
	long operator()(double, long);
	float operator()(double, int)const;
	double operator()(double, long)const;

	static constexpr size_t max(size_t lhs, size_t rhs) {
		return lhs < rhs ? rhs : lhs;
	}

	template<class...Args>
	auto operator()(Args...args) {
		return select<max(sizeof...(args), (size_t)1) - 1>(1, args...);
	}

};

int bar(int, double, float);






struct Print
{
	template<typename T>
	void operator () (T&& t)
	{
		std::cout << t << " ";
	}
};
template<class F, size_t n = 1>
struct RepeatHelper : F {
	template<class...Ts>
	decltype(auto) operator()(Ts&&...args) {
		return repeat<n>((F&)*this, forward_m(args)...);
	}
};

#if use_cpp11
template<class F, class RefType>
struct RepeatAuto
{
	F& func_m;
	RepeatAuto(RefType f) :func_m{ f } {}
	template<class...Ts>
	decltype(auto) operator()(Ts&&...args) {
		using param_parser = decltype(countParamShortest(std::declval<F>(), forward_m(args)...));
		return repeat<SeqSize<param_parser>::value>((RefType&)func_m, forward_m(args)...);
	}
};

template<class F, class...Ts>
decltype(auto) repeatAuto(F&& func, Ts&&...args) {
	RepeatAuto<std::remove_reference_t<F>, decltype(func)>{func}(forward_m(args)...);
}
#else
template<class F, class...Ts>
decltype(auto) repeatAuto(F&& func, Ts&&...args) {
	//note: We have to move it into lambda body or it would cause a gcc bug.(Segment fault)
	//param_parser=decltype(ShortParser{}(forward_m(func_m), forward_m(args_m)...));
	return [](auto&& func_m, auto&&...args_m) {
		using param_parser = decltype(ShortParser{}(forward_m(func_m), forward_m(args_m)...));
		return repeat<SeqSize<param_parser>::value - 1>(forward_m(func_m), forward_m(args_m)...);
	}(forward_m(func), forward_m(args)...);
}



#endif


using PrintAll = RepeatHelper<Print>;

template<typename... Ts>
void split_and_print(Ts&&... args)
{
	constexpr size_t packSize = sizeof...(args);
	constexpr size_t halfSize = packSize / 2;

	std::cout << "Printing first half:" << std::endl;
	applyFront<halfSize>(PrintAll{}, forward_m(args)...);

	std::cout << "Printing second half:" << std::endl;
	applyBack<halfSize>(PrintAll{}, forward_m(args)...);
}
template<unsigned I, typename... Ts>
void print_first_last_and_indexed(Ts&&... args)
{
	std::cout << "First argument: " << get<0>(forward_m(args)...) << std::endl;
	std::cout << "Last argument: " << get<sizeof...(Ts)-1>(forward_m(args)...) << std::endl;
	std::cout << "Argument #" << I << ": " << get<I>(forward_m(args)...) << std::endl;
}


int main2() {
	repeatAny(Printer2{}, 1, 3, 3, 4, 5, 7.0, 88, 999, 31.2353543, 23);
	repeatAuto(Print{}, 1, 2, 3, 4, 5);
	apply(std::make_index_sequence<4>{}, Printer2{}, 1, 2, 3, 4, 5, 6, 7, 8);
	applyRag<2, 5>([](auto&&...args) {
		PrintAll{}(forward_m(args)...);
	}, 0, 1, 2, 3, 4, 5, 6, 7, 8);
	PrintAll{}(0, 1, 2, 3, 4, 5, 6, 7, 8);
	print_first_last_and_indexed<5>(0, 1, 2, 3, 4, 5, 6, 7, 8);
	split_and_print(0, 1, 2, 3, 4, 5, 6, 7, 8);
	return 0;
}
#if 0
int main() {
	unsigned count = 0;
	auto res = repeat<2>([&count](int lhs, int rhs) {
		std::cout << "(" << lhs << " , " << rhs << ")" << std::endl;
		++count;
		return count * 2;
	}, 1, 2, 3, 4, 5, 6);
	std::cout << "To output " << res << " value, "
		<< "we can called a function repeatly with 2 parameters in "
		<< count << " times total." << std::endl;

	auto print_each = [](auto&&...args) {
		repeat<1>(print{}, forward_m(args)...);
	};
	std::cout << "Also we can print all values in once:\n";
	print_each(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 3.14159);

	std::cout << "\nor print 6 values first:\n";
	applyFront<6>(print_each, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 3.14159);
	std::cout << "\nand then print other values:\n";
	applyBack<6>(print_each, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 3.14159);
	return 0;
}
#endif

int main3() {

	{
		auto f = [](int a) {
			std::cout << a << " ";
			return a;
		};
		auto mf = [&f](auto&&...a) {
			return repeat<1>(f, forward_m(a)...);
		};
		std::cout << repeat<1>(f, 1, 2, 3) << " ";
		apply(std::index_sequence<0, 2, 4, 6>{}, mf, 1, 2, 3, 4, 5, 6, 7);
	}

	{
		auto f = [](auto&&...args) {
			repeat<1>([](auto&& arg) {
				std::cout << forward_m(arg) << "\n";
			}, forward_m(args)...);
		};
		applyFront<6>(f, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
		std::cout << "-----------\n";
		applyBack<6>(f, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	}

	/*{
	using namespace std;
	using boost::typeindex::type_id_with_cvr;
	int a = 0;
	const string str{ "hello" };
	auto func = [](auto&& lhs, auto&& rhs) {
	std::cout << type_id_with_cvr<decltype(lhs)>().pretty_name()
	<< ":" << forward_m(lhs) << "\n"
	<< type_id_with_cvr<decltype(rhs)>().pretty_name()
	<< ":" << forward_m(rhs) << "\n";
	};
	func(0, 1);
	repeat<2>(func,
	a, 1,
	2, 3,
	4, 5,
	6,
	"c string\n",
	"string\n"s,
	str);

	}*/

#if 0
	Printer1 pr;
	Printer2 pr2;
	const Printer2 cpr2;
	std::cout << "direct call----------------------\n";
	pr(1);
	pr(2.0);
	pr(3);
	pr(3.333f);
	std::cout << "use repeat------------------------\n";
	pr(1, 2.0, 3, 3.333f, "-----\n");


	pr2(1, 2.0, 3, false, "-----\n");
	cpr2(1, 2.0, 3, true, "-----\n");
	repeat<1>([&pr2](auto&&arg) {pr2(forward_m(arg)); },
		4.0, -5, 6, -7, "string");
#endif // 0
	//#include "testBeforeAtAfter.h"


	Foo f{};


	ShortFirstCall<Seq<int>, int, int>{};

	//decltype(bar1((C*)0,1,2,3)) p6=f;
	Is_Same(Seq<char, int>, decltype(ShortParser{}(f, 1)));
	using namespace std::placeholders;
	auto bind = [](auto&&... args) {return std::bind(forward_m(args)...); };

	Is_Same(COUNT_RES(bind, f, _1, _3, _2),
		decltype(std::bind(f, _1, _3, _2)));

	//decltype(LongParser{}(f,1,1.0,2,3)) *p6=f;
	//f(1,1.0);
	//decltype(LongParser{}(f,1,1.0))*p6=f;
	Is_Same(decltype(f('a', 1)), COUNT_RES(f, 'a', 1));
	Is_Same(decltype(f('a', 1.0, 21)), COUNT_RES(f, 'a', 1.0, 2l));
	Is_Same(decltype(f(1.0, 2l)), COUNT_RES(f, 1.0, 2l));
	Is_Same(decltype(f(1.0, 2)), COUNT_RES(f, 1.0, 2));
	Seq< At<0, Seq<int&, double, float>>,
		At<1, Seq<int, double&, float>>,
		At<2, Seq<int, double, float&&>>  >*p = 0;
	Reverse<std::remove_reference_t<decltype(*p)>>* p2 = 0;

	Erase_front<int, Seq<int, double, float>> * p3 = 0;

	int a = 0;
	select<0>(a, 1, 3, 4, 3.9) = 10;
	std::cout << "a=" << a << std::endl;

	int b = select<0>(1.3, 23, 3.5, 7.8);
	a = b;


	Is_Same(RotateLeft<int, Seq<int, double, float>>,
		Seq<double, float, int>);

	Is_Same(RotateLeft<Seq<int, double>, Seq<int, double>>,
		Seq<int, double>);

	Is_Same(Find_t<int, Seq<double, double, int, float, long, char, int>>,
		Seq<float, long, char, int>);

	Is_Same(Find_t<int, Seq<long, char, double, int>>,
		Seq<>);


	static_assert(Find<long, Seq<long, char, double, int>>::value == 0, "");
	static_assert(Find<char, Seq<long, char, double, int>>::value == 1, "");
	static_assert(Find<double, Seq<long, char, double, int>>::value == 2, "");
	static_assert(Find<int, Seq<long, char, double, int>>::value == 3, "");
	static_assert(Find<float, Seq<long, char, double, int>>::value == no_index, "");

	Is_Same(Find_t<int, Seq<char, char>>,
		Seq<NoType>);

	Is_Same(Multiop<Find, Seq<int, float>, Seq<double, float, long, char, int>>,
		Seq<long, char, int>);

	Is_Same(Merge<Seq<int>, Seq<>>, Seq<int>);

	Multiop<Find, Seq<int, char, char>, Seq<double, float, long, int, double>>* p4 = 0;
	Multiop_n<Find, Seq<int, double, float>, Seq<double, float, long, int, double>>* p5 = 0;
	return 0;
}







/***
struct FindCon<Seq<T,Ts...>,Seq<Us...>>{
using type=Merge_s<
Find<
T,
Seq<Us...>
>,
FindCon<
Seq<Ts...>,
Seq<Us...>
>
>;
};***/









