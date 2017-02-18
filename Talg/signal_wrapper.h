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
	//static U declParam();
	//static int f(T);
	//static void f(...);
	public:
		static constexpr bool value = std::is_same<std::decay_t<U>, std::decay_t<T>>::value
			&& std::is_convertible<U,T>::value;
	};
};

template<class T,class S>
using FindParam = Find_if_svt<MatchParam, T, S>;







/*
	\brief	辅助类型,用特化来延迟类型生成,直接使用conditional的话无法达成此效果.
	\param	T将被适配的用户函子,StandarType标准回调类型,参见FunctorImp
	\return	参数的索引号,例如 IdSeq<0,2,1,3>
*/
template<class T,class StandarType,bool is_direct_invoke>
struct MapParamIdImp{
	using UserFunParam = typename CallableTraits<T>::arg_type;
	using StandarParam = typename CallableTraits<StandarType>::arg_type;
	using type = std::conditional_t<std::is_same<UserFunParam,StandarParam>::value,
		Seq<>,
		Multiop_n<FindParam,UserFunParam,StandarParam>
	>;
};
template<class T,class U>
struct MapParamIdImp<T,U,true>{
	using type = Seq<>;
};
template<class T,class StandarType>
using MapParamId = typename MapParamIdImp<T, StandarType,
		std::is_same<T, StandarType>::value || !isNonOverloadFunctor<T>::value
		//在is_same成立时直接invoke节省编译时间,
		//在没有可能推断出参数类型时(比如一个函数对象有多个operator())也只能直接invoke
	>::type;

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
	using ParameterIndex = MapParamId<T, StandarType>;
	constexpr const T& getFunc()const noexcept{
		return std::get<0>(*this);
	}
	T& getFunc()noexcept{
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
		//StaticAssert<NotSame<ParameterIndex, IdSeq<>>, T, ParameterIndex,CallableTraits<T>> {};
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
	\brief	函数签名限定
	\param	Derived派生类,用于转换. 
			Ret限定函数返回类型,
			Ps...限定函数参数类型
	\note	Ps或Ret皆可能是引用
			特别使用template<class...>
			来使transform可行,然后再用特化,
			如果不是如此FnSig不可作为template<class...>class传递
			进而无法transform
*/
template<class...>struct FnSig;

template<class Derived,class Ret,class...Ps>
struct FnSig<Derived,Ret,Ps...> {
	Ret operator()(Ps...args) {
			return static_cast<Derived*>(this)->call(forward_m(args)...);
	}
	constexpr Ret operator()(Ps...args)const{
		return static_cast<const Derived*>(this)->call(forward_m(args)...);
	}

};



template<class Ptr,class Pmd,class StandarT>
struct MemFun
{
	using ParameterIndex = MapParamId<Pmd, StandarT>;

	static_assert(std::is_pointer<Ptr>::value, "");
	Ptr ptr_;
	Pmd pmd;
	constexpr MemFun(Ptr obj,Pmd ptr):ptr_(forward_m(obj)),pmd(ptr){}


	template<class...Ts>
	constexpr decltype(auto) operator()(Ts&&...args)const{
		return applyMemFun(ParameterIndex{},ptr_, pmd, forward_m(args)...);
	}

	template<class...Ts>
	decltype(auto) operator()(Ts&&...args) {
		return applyMemFun(ParameterIndex{},ptr_, pmd, forward_m(args)...);
	}

	template<class Other,class DataU,class S>
	constexpr bool operator==(const MemFun<Other, DataU,S>& rhs)const {
		if (rhs.ptr_ == nullptr) {
			return pmd == rhs.pmd;
		} else if (rhs.pmd == nullptr) {
			return ptr_ = rhs.ptr_;
		} else {
			return ptr_ == rhs.ptr_ && pmd == rhs.pmd;
		}
	}
};

template<class T,class DataT,class StandarT>
struct MemFun<std::weak_ptr<T>,DataT,StandarT>
{
	using ParameterIndex = MapParamId<DataT, StandarT>;

	std::weak_ptr<T> ptr_;
	DataT T::*pmd;
	template<class U,class P>
	constexpr MemFun(U&& obj,P ptr):ptr_(forward_m(obj)),pmd(ptr){}

	template<class...Ts>
	void operator()(Ts&&...args) {
		if(auto ptr = ptr_.lock()){
			applyMemFun(ParameterIndex{},ptr_, pmd, forward_m(args)...);
		}
	}
	template<class...Ts>
	void operator()(Ts&&...args)const{
		if(auto ptr = ptr_.lock()){
			applyMemFun(ParameterIndex{},ptr_, pmd, forward_m(args)...);
		}
	}
	template<class Other,class DataU,class S>
	constexpr bool operator==(const MemFun<Other, DataU,S>& rhs)const {
		if (rhs.ptr_ == nullptr) {
			return pmd == rhs.pmd;
		} else if (rhs.pmd == nullptr) {
			return ptr_ = rhs.ptr_;
		} else {
			return ptr_ == rhs.ptr_ && pmd == rhs.pmd;
		}
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

template<class StandarType, class T,class Pmd>
constexpr decltype(auto) makeFunctor(T* obj,Pmd pmd) {
	return MemFun<T*,Pmd,StandarType>{obj, pmd};
}


template<class StandarType,class T,class Pmd>
decltype(auto) makeFunctor(std::weak_ptr<T> ptr,Pmd pmd) {
	return	MemFun<std::weak_ptr<T>,Pmd,StandarType>{ptr, pmd};
}



/*
	\brief	提供各种语法糖,如有必要可以切换至boost signals2
	\param	模板参数 形如Signal<R(Ts...),Us...>
	\note	直接采用了无节操的公有继承,嗯,所谓相信程序员...??!
*/
template<template<class...>class Sig,class...Ts> 
class SignalWrapper:public Sig<Ts...>
{
	using Base = Sig<Ts...>;
public:
	using ftype = Head_s<Seq<Ts...>>;
	using Base::Base;
	using Base::empty;
	//using Base::operator();
public:
	/*
		\brief	添加函数到信号槽
		\param	func任意的函数对象,支持可选参数
				(无视顺序,但要求去除cv后必须类型一致,具体见functor),
		\return	返回对象自身,从而支持链式调用如
				sig += f1
					+= f2;
		\note	使用者必须当心链式调用时的参数求值顺序不确定
	*/
	template<class F>
	SignalWrapper& operator+=(F&& func) 
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		Base::connect(makeFunctor<ftype>(forward_m(func)));
		return *this;
	}
	/*
		\brief	去除与一个与指定的函数对象相等的槽.
		\param	func任意的函数对象,支持可选参数
				(无视顺序,但要求去除cv后必须类型一致,具体见functor)
				必须要与添加时的类型一致才可能成功删除.
		\return	返回对象自身,从而支持链式调用如
				sig -= f1
					-= f2;
		\note	使用者必须当心链式调用时的参数求值顺序不确定
		\todo fix	boost好像是全部去除,与此处行为不一致.
					提供利用函数对象指针来删除成员函数回调的重载.
	*/
	template<class F>
	SignalWrapper& operator-=(F&& func)
		except_when(std::declval<Base&>().disconnect(makeFunctor<ftype>(forward_m(func))))
	{
		Base::disconnect(makeFunctor<ftype>(forward_m(func)));
		return *this;
	}

	/*
		\brief	 删除某个回调
		\note	直接用functor进行包装,从而与connect时的参数类型相称,
				对于不提供等号比较的类型(比如lambda),直接比较类型.
				对于成员函数可以直接 connect(obj,&Obj::mem_func);
	*/
	template<class... Fs>
	decltype(auto) disconnect(Fs&&... func)
		except_when(std::declval<Base&>().disconnect(makeFunctor<ftype>(forward_m(func)...)))
	{
		return Base::disconnect(makeFunctor<ftype>(forward_m(func)...));
	}
	template<class... Fs>
	decltype(auto) disconnect_all(Fs&&... func)
		except_when(std::declval<Base&>().disconnect_all(makeFunctor<ftype>(forward_m(func)...)))
	{
		return Base::disconnect_all(makeFunctor<ftype>(forward_m(func)...));
	}


	/*
		\brief	添加回调
		\note	直接用functor进行包装,从而提供可选参数支持,
				对于成员函数可以直接 connect(obj,&Obj::mem_func);
	*/
	template<class... Fs>
	decltype(auto) connect(Fs&&... func) 
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func)...)))
	{
		return Base::connect(makeFunctor<ftype>(forward_m(func)...));
	}


	/*
		\brief	提供便利的语法来返回connection
		\param	同operator+=
		\return 同connect
		\note	不满足交换, func+sig 的写法会导致编译错误
	*/
	template<class F>
	decltype(auto) operator+(F&& func) { 
		return connect(forward_m(func));
	}

	decltype(auto) disconnect_all()
		except_when(std::declval<Base&>().disconnect_all())
	{
		return Base::disconnect_all();
	}
};


#endif // !SIGNAL_WRAPPER_H_INCLUDED

