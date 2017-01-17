#pragma once
#include "seqop.h"
#include <utility>
#include <type_traits>
#include <functional>



#if defined(_MSC_VER)
	#if _MSC_VER>=1900
		using std::invoke;
	#endif
#elif  defined(__cpp_lib_invoke)
	using std::invoke;
#else
namespace TalgDetail {
	template <class T>
	struct is_reference_wrapper : std::false_type {};
	template <class U>
	struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};


	template <class Base, class T, class Derived, class... Args>
	auto INVOKE(T Base::*pmf, Derived&& ref, Args&&... args)
		noexcept(noexcept((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...)))
		-> std::enable_if_t<std::is_function<T>::value &&
		std::is_base_of<Base, std::decay_t<Derived>>::value,
		decltype((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...))>
	{
		return (std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...);
	}

	template <class Base, class T, class RefWrap, class... Args>
	auto INVOKE(T Base::*pmf, RefWrap&& ref, Args&&... args)
		noexcept(noexcept((ref.get().*pmf)(std::forward<Args>(args)...)))
		-> std::enable_if_t<std::is_function<T>::value &&
		is_reference_wrapper<std::decay_t<RefWrap>>::value,
		decltype((ref.get().*pmf)(std::forward<Args>(args)...))>

	{
		return (ref.get().*pmf)(std::forward<Args>(args)...);
	}

	template <class Base, class T, class Pointer, class... Args>
	auto INVOKE(T Base::*pmf, Pointer&& ptr, Args&&... args)
		noexcept(noexcept(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...)))
		-> std::enable_if_t<std::is_function<T>::value &&
		!is_reference_wrapper<std::decay_t<Pointer>>::value &&
		!std::is_base_of<Base, std::decay_t<Pointer>>::value,
		decltype(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...))>
	{
		return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
	}

	template <class Base, class T, class Derived>
	auto INVOKE(T Base::*pmd, Derived&& ref)
		noexcept(noexcept(std::forward<Derived>(ref).*pmd))
		-> std::enable_if_t<!std::is_function<T>::value &&
		std::is_base_of<Base, std::decay_t<Derived>>::value,
		decltype(std::forward<Derived>(ref).*pmd)>
	{
		return std::forward<Derived>(ref).*pmd;
	}

	template <class Base, class T, class RefWrap>
	auto INVOKE(T Base::*pmd, RefWrap&& ref)
		noexcept(noexcept(ref.get().*pmd))
		-> std::enable_if_t<!std::is_function<T>::value &&
		is_reference_wrapper<std::decay_t<RefWrap>>::value,
		decltype(ref.get().*pmd)>
	{
		return ref.get().*pmd;
	}

	template <class Base, class T, class Pointer>
	auto INVOKE(T Base::*pmd, Pointer&& ptr)
		noexcept(noexcept((*std::forward<Pointer>(ptr)).*pmd))
		-> std::enable_if_t<!std::is_function<T>::value &&
		!is_reference_wrapper<std::decay_t<Pointer>>::value &&
		!std::is_base_of<Base, std::decay_t<Pointer>>::value,
		decltype((*std::forward<Pointer>(ptr)).*pmd)>
	{
		return (*std::forward<Pointer>(ptr)).*pmd;
	}

	template <class F, class... Args>
	auto INVOKE(F&& f, Args&&... args)
		noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
		-> std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>::value,
		decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
	{
		return std::forward<F>(f)(std::forward<Args>(args)...);
	}
} // namespace detail

template< class F, class... ArgTypes >
constexpr decltype(auto) invoke(F&& f, ArgTypes&&... args)
noexcept(noexcept(TalgDetail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...)))
{
	return TalgDetail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...);
}

#endif // !__cpp_lib_invoke





template<class...Ts>struct LongFirstCall;

template<class...Ts>
struct LongFirstCall<Seq<Ts...>> :public LongFirstCall<Pop_back_s<Seq<Ts...>>> {
	using LongFirstCall<Pop_back_s<Seq<Ts...>>>::pcall;
	using self = LongFirstCall<Seq<Ts...>>;

	//Us&&...的目的是让所有pcall都能以同样的方式调用
	//self* 是为了利用派生类到基类的转换顺序来逐个sfinae掉不合法的调用
	template<class F, class...Us>
	auto pcall(self*, F&& f, Ts&&...args, Us&&...)
		->Seq< decltype(forward_m(f)(forward_m(args)...)), Ts...>;
	//返回的类型为 Seq<返回值类型, 参数类型>
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


struct EatParam {
	constexpr EatParam(...)noexcept {}
};
template<class>
using EatParam_t = EatParam;

template<class...Ts>
struct GetImp {
	
	template<class T>
	static constexpr decltype(auto) fetch(EatParam_t<Ts>&&..., T&& obj, ...)noexcept {
		return std::forward<T>(obj);
	}
	template<class T>
	static WrapperT<T> deduce(Ts..., Seq<T>* obj, ...);

	template<class F, class...Us>
	static constexpr decltype(auto) applyFrontImp(F&& func, Ts&&...args, Us&&...)
		noexcept(noexcept(forward_m(func)(forward_m(args)...)))
	{
		return forward_m(func)(forward_m(args)...);
	}

	template<class F, class...Us>
	static constexpr decltype(auto) applyBackImp(F&& func, Ts&&..., Us&&...args)
		noexcept(noexcept(forward_m(func)(forward_m(args)...)))
	{
		return forward_m(func)(forward_m(args)...);
	}

	template<class...Us>
	static constexpr auto subSeq(Ts&&...args, Us&&...)
		noexcept(noexcept(std::forward_as_tuple(forward_m(args)...)))
	{
		return std::forward_as_tuple(forward_m(args)...);
	}
};





template<size_t n>
struct IgnoreSeqImp {
	using type = Merge_s<OMIT_T(IgnoreSeqImp<n / 2>), OMIT_T(IgnoreSeqImp<n - n / 2>)>;
};
template<>
struct IgnoreSeqImp<0> {
	using type = Seq<>;
};
template<>
struct IgnoreSeqImp<1> {
	using type = Seq<EatParam>;
};

template<size_t n>
using IgnoreSeq = OMIT_T(IgnoreSeqImp<n>);


template<size_t n>
using ExcludeParam = Transform<GetImp, IgnoreSeq<n>>;

template<size_t n, class...Ts>
constexpr decltype(auto) get(Ts&&...args)noexcept {
	static_assert(n<sizeof...(args), "index over range.");
	return ExcludeParam<n>::fetch(forward_m(args)...);
}



template<size_t n, class T>struct AtImp;
template<size_t n, class...Ts>
struct AtImp<n, Seq<Ts...>> {
	using tmp = decltype(
		ExcludeParam<n>::deduce(static_cast<Seq<Ts>*>(0)...)
		);
	using type = typename tmp::type;
};
template<size_t n, class T>
using At_s = OMIT_T(AtImp<n, assert_is_seq(T)>) ;
template<size_t n, class T>
using At = OMIT_T(AtImp<n, Seqfy<T>>);


template<size_t n, class F, class...Ts>
decltype(auto) applyFront(F&& func, Ts&&... args) {
	return Transform<GetImp, Before_s<n, Seq<Ts...>> >
		::applyFrontImp(forward_m(func), forward_m(args)...);
}
template<size_t n, class F, class...Ts>
decltype(auto) applyBack(F&& func, Ts&&... args) {
	return ExcludeParam<n>::applyBackImp(forward_m(func), forward_m(args)...);
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
	return Transform<ApplyRagImp, IgnoreSeq<beg> >
		::call(std::index_sequence<end - beg >{}, forward_m(func), forward_m(args)...);
}


template<size_t... Indices, class F, class...Ts>
constexpr decltype(auto) apply(std::index_sequence<Indices...>, F&& func, Ts&&... args) {
	return invoke(forward_m(func), get<Indices>(forward_m(args)...)...);
}
template<class F, class...Ts>
constexpr decltype(auto) apply(std::index_sequence<>, F&& func, Ts&&... args) {
	return forward_m(func)();  
}
template<class F, class...Ts>
constexpr decltype(auto) apply(Seq<>, F&& func, Ts&&... args) {
	return invoke(forward_m(func), forward_m(args)...);
}







template<class F, class...Ts>
void mapAny(F&& func, Ts&&...args);


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
	static void mapAny(F&& func, Ts&&...args, Us&&...last) {
		forward_m(func)(forward_m(args)...);
		mapAny(forward_m(func), forward_m(last)...);
	}
	template<class F, class...Us>
	static void mapAny(F&& func, Ts&&...args) {
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
void mapAny(F&& func, Ts&&...args) {
	using param_parser = decltype(ShortParser{}(forward_m(func), forward_m(args)...));
	constexpr size_t param_length = SeqSize<param_parser>::value - 1;
	using begin = Before_s<param_length, Seq<Ts...>>;

	return Transform<GetSeqImp, begin>::mapAny(forward_m(func), forward_m(args)...);
}


