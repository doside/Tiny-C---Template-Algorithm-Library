#ifndef SIGNAL_WRAPPER_H_INCLUDED
#define SIGNAL_WRAPPER_H_INCLUDED



#include "callable_traits.h"
#include "find_val.h"
#include "select_type.h"
#include "has_member.h"
#include <type_traits>	
#include <tuple>

struct MatchParam
{
	template<class T, class U>
	struct pred
	{
	private:
		static U declParam();
		static int f(T);
		static void f(...);
	public:
		static constexpr bool value = std::is_same<std::decay_t<U>, std::decay_t<T>>::value
			&& std::is_convertible<U,T>::value;
	};
};

template<class T,class S>
using FindParam = Find_if_svt<MatchParam, T, S>;





template<class T,bool=std::is_empty<T>::value>
struct FunctorBaseObj {
	T val;
	template<class U>
	constexpr FunctorBaseObj(U&& v):val(forward_m(v)){}
	constexpr const T& get()const noexcept { return val; }
};
template<class T>
struct FunctorBaseObj<T,true>:T{	//for empty base optimistion
	template<class U>
	constexpr FunctorBaseObj(U&& v):T(forward_m(v)){}
	constexpr const T& get()const noexcept { return *this; }
};



/*
	\brief	把T类型的函数对象适配成StandarType,并且提供相等比较.
	\param	T 原函子,StandarType要适配成的类型
	\return	可以像调用StandarType那样调用T
	\note	T的参数必须比StandarType的参数个数少.
			T的参数类型与StandarType的参数类型必须在decay后完全一致,
			并且后者必须可以转换成前者(因为是以StandarType的参数调用前者)
			关于相等运算的语义参见\operator==
*/
template<class T, class StandarType>
struct FunctorImp:FunctorBaseObj<T>{
	using Base = FunctorBaseObj<T>;	
	static_assert(!std::is_reference<T>::value, "we assume T is not a ref.");
	using ParameterIndex = 
		std::conditional_t<std::is_same<T,StandarType>::value,
		Seq<>,
		Multiop_n<FindParam,
			typename CallableTraits<T>::arg_type,
			typename CallableTraits<StandarType>::arg_type
		>
	>;
	constexpr const T& getFunc()const noexcept{
		//return std::get<0>(static_cast<const Base&>(*this));
		return Base::get();
	}



public:
	constexpr FunctorImp(const T& f)noexcept(std::is_nothrow_copy_constructible<T>::value)
		:Base(f)
	{

	}
	constexpr FunctorImp(T&& f)noexcept(std::is_nothrow_move_constructible<T>::value)
		: Base(std::move(f))
	{

	}

	template<class...Ts>
	decltype(auto) operator()(Ts&&...args)
		noexcept(noexcept(
			apply(ParameterIndex{},std::declval<T>(), forward_m(args)...)
		))
	{
		return apply(ParameterIndex{},getFunc(), forward_m(args)...);
	}


	template<class...Ts>
	constexpr decltype(auto) operator()(Ts&&...args)const
		noexcept(noexcept(
			apply(ParameterIndex{},std::declval<T>(), forward_m(args)...)
		))
	{
		return apply(ParameterIndex{}, getFunc(), forward_m(args)...);
	}
	

	/*
		\brief	相等比较
		\note	当用户类型提供有相等比较(必须支持const)时,则采用它,
				否则直接根据类型的异同进行判断,如果两者的StandarType不同则引发编译错误.
				
		\todo	如果用户类型是std::function是否应该比较target？
				当用户类型提供有相等比较时,是否应该允许与非StandarType的函数对象比较？
	*/
	template<class U,class S,
		class= std::enable_if_t< hasEqualCompare<const T&,const U&>::value >
	>
	constexpr bool operator==(const FunctorImp<U,S>& rhs)const
		except_when(std::declval<T>()== std::declval<U>())
		//->decltype(std::declval<const T&>()== std::declval<const U&>())
	{
		//这个断言是为了保持一致性,实际上当用户类型提供有相等比较时可以放宽
		static_assert(std::is_same<S, StandarType>::value,
			"It is not allowed to compare two functor which have different StandarType.");
		return	getFunc() == rhs.getFunc();
	}

	template<class F>
	constexpr bool operator==(const FunctorImp<F,StandarType>& rhs)const noexcept
	{
		return std::is_same<T, F>::value;// &&std::is_same<StandarType, S>::value;
	}

	template<class F,class S>
	constexpr bool operator!=(const FunctorImp<F,S>& rhs)const
		noexcept(noexcept(std::declval<const FunctorImp&>().operator==(rhs)))
	{
		return !operator==(rhs);
	}

};



template<class F, class T>
constexpr decltype(auto) makeFunctor(T&& src_func) {
	return FunctorImp<std::remove_reference_t<T>, F>(forward_m(src_func));
}
template<class F,class R,class...Ps>
constexpr decltype(auto) makeFunctor(R (*src_func)(Ps...)) {
	return FunctorImp<decltype(src_func), F>(src_func);
}


template<template<class...>class Sig, class...Ts> 
class SignalWrapper:public Sig<Ts...>
{
	using Base = Sig<Ts...>;
public:
	using ftype = Head_s<Seq<Ts...>>;
	using Base::Base;
	template<class R,class...Ps>
	decltype(auto) operator+=(R(*func)(Ps...))
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(func)))
	{
		return Base::connect(makeFunctor<ftype>(func));
	}
	template<class F>
	decltype(auto) operator+=(F&& func) 
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::connect(makeFunctor<ftype>(forward_m(func)));
	}
	template<class F>
	decltype(auto) operator-=(F&& func)
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::disconnect(makeFunctor<ftype>(forward_m(func)));
	}

};

#endif // !SIGNAL_WRAPPER_H_INCLUDED

