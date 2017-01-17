#include "core.h"
#include "invoke_impl.h"

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
		noexcept(noexcept(invoke(forward_m(func), forward_m(args)...)))
	{
		return invoke( forward_m(func),forward_m(args)...);
	}
	template<class F, class...Us>
	static constexpr decltype(auto) applyBackImp(F&& func, Ts&&..., Us&&...args)
		noexcept(noexcept(invoke(forward_m(func), forward_m(args)...)))
	{
		return invoke(forward_m(func), forward_m(args)...);
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
using At_s = OMIT_T(AtImp<n, T>);
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
