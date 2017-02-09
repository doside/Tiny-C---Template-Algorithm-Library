#ifndef SIGNAL_WRAPPER_H_INCLUDED
#define SIGNAL_WRAPPER_H_INCLUDED



#include "callable_traits.h"
#include "find_val.h"
#include "select_type.h"
#include "has_member.h"
#include <type_traits>	
#include <tuple>
#include <forward_list>

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




template<class Func>
struct EqualableFunction :std::function<Func> {
	using Base = std::function<Func>;
	using Base::Base;
	template<class F>
	bool operator==(const F& rhs)const 
		except_when(std::declval<const F&>()==std::declval<const F&>())
	{
		auto ptr = Base::template target<F>();
		if (hasEqualCompare<const F&>::value) {
			return *ptr==rhs;
		}
		return ptr != nullptr;
	}
	template<class F>
	constexpr bool operator==(const std::function<F>& rhs)const noexcept
	{
		static_assert(std::is_same<const std::function<F>&,decltype(rhs)>::value,
					"Can't support such comparation");
		return false;
	}

	template<class F>
	constexpr bool operator!=(const F& rhs)const
		except_when(std::declval<EqualableFunction<Func>>()==rhs)
	{
		return !operator==(rhs);
	}
};



template<class T,class StandarType,bool>
struct FunctorImpHelper{
	using type = Multiop_n<FindParam,
		typename CallableTraits<T>::arg_type,
		typename CallableTraits<StandarType>::arg_type
	>;
};
template<class T,class U>
struct FunctorImpHelper<T,U,true>{
	using type = Seq<>;
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
struct FunctorImp:private std::tuple<T>{	//派生有助于空基类优化,但由于T可能是基础类型,无法直接从其派生
	using Base = std::tuple<T>;	
	static_assert(!std::is_reference<T>::value, "we assume T is not a ref.");
	using ParameterIndex = typename FunctorImpHelper<T, StandarType,
		std::is_same<T, StandarType>::value || !isNonOverloadFunctor<T>(nullptr)
	>::type;
	constexpr const T& getFunc()const noexcept{
		return std::get<0>(*this);
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
	template<class U,
		class= std::enable_if_t< hasEqualCompare<const T&,const U&>::value >
	>
	constexpr bool operator==(const FunctorImp<U,StandarType>& rhs)const
		except_when(std::declval<T>()== std::declval<U>())
		//->decltype(std::declval<const T&>()== std::declval<const U&>())
	{

		return	getFunc() == rhs.getFunc();
	}

	template<class F,class S>
	constexpr bool operator==(const FunctorImp<F,S>& rhs)const noexcept
	{
		return std::is_same<T, F>::value && std::is_same<StandarType, S>::value;
	}

	template<class F,class S>
	constexpr bool operator!=(const FunctorImp<F,S>& rhs)const
		noexcept(noexcept(std::declval<const FunctorImp&>().operator==(rhs)))
	{
		return !operator==(rhs);
	}

};




/*
	\brief	如果可以分析函子的参数表,那么就提供StandarType到其的转化,否则直接保存.
	\param	StandarType标准的回调类型,T需要被调整以适应标准型的函数对象.
	\return 返回适当调整后的函数对象,它符合标准回调类型,并且可比较相等.
	\note	T的参数个数必须比标准型少,少掉的参数视作忽略.
	\todo	提供对成员函数指针的特别支持.
*/
template<class StandarType, class T>
constexpr decltype(auto) makeFunctor(T&& src_func) {
	return FunctorImp<std::remove_reference_t<T>, StandarType>(forward_m(src_func));
}
template<class StandarType,class R,class...Ps>
constexpr decltype(auto) makeFunctor(R (*src_func)(Ps...)) {
	return FunctorImp<decltype(src_func), StandarType>(src_func);
}


template<template<class...>class Sig,class...Ts> 
class SignalWrapper:private Sig<Ts...>
{
	using Base = Sig<Ts...>;
public:
	using ftype = Head_s<Seq<Ts...>>;
	using Base::Base;
	using Base::operator();
	
public:
	template<class F>
	decltype(auto) operator+=(F&& func) 
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::connect(makeFunctor<ftype>(forward_m(func)));
	}


	template<class F>
	decltype(auto) operator-=(F&& func)
		except_when(std::declval<Base&>().disconnect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::disconnect(makeFunctor<ftype>(forward_m(func)));
	}


	template<class F>
	decltype(auto) disconnect(F&& func)
		except_when(std::declval<Base&>().disconnect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::disconnect(makeFunctor<ftype>(forward_m(func)));
	}
	template<class F>
	decltype(auto) disconnect_all(F&& func)
		except_when(std::declval<Base&>().disconnect_all(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::disconnect_all(makeFunctor<ftype>(forward_m(func)));
	}
	template<class F>
	decltype(auto) connect(F&& func) 
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::connect(makeFunctor<ftype>(forward_m(func)));
	}
};

template<class Func>
class BasicSignal {
	using SlotType = EqualableFunction<Func>;
	using container = std::forward_list<SlotType>;
	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;
	container slot_list;
	iterator last;
public:
	struct Connection{
		iterator node;
		container* ref=nullptr;
		Connection(iterator link,container& src):node(link),ref(&src){}
		Connection(){}
		void disconnect() {
			ref->erase_after(node);
		}
	};
public:
	BasicSignal()
	:slot_list(),last(slot_list.before_begin()){}

	template<class T>
	Connection connect(T&& func) {
		slot_list.insert_after(last,forward_m(func));
		return Connection(last++, slot_list);
	}
	template<class...Ts>
	void operator()(Ts&&...args) {
		for(auto&elem:slot_list){
			elem(forward_m(args)...);
		}
	}
	template<class F>
	void disconnect(F&& func) {
		auto iter = slot_list.cbegin();
		auto prev = slot_list.cbefore_begin();
		for(;iter!=slot_list.cend();++iter){
			if (*iter==forward_m(func)) {
				slot_list.erase_after(prev);
				break;
			}
			prev = iter;
		}
	}
	template<class F>
	void disconnect_all(F&& func) {
		slot_list.remove(forward_m(func));
	}

};

template<class...Ts>
using SimpleSignal = SignalWrapper<BasicSignal, Ts...>;

#endif // !SIGNAL_WRAPPER_H_INCLUDED

