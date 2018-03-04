#ifndef SIGNAL_WRAPPER_H_INCLUDED
#define SIGNAL_WRAPPER_H_INCLUDED

#include "function_wrapper.h"
#include "basic_marco_impl.h"

namespace Talg{
/*
	\brief	提供各种语法糖,如有必要可以切换至boost signals2
	\param	模板参数 形如Signal<R(Ts...),Us...>
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
		\note	使用者必须当心链式调用时,参数求值顺序是不确定的.
	*/
	template<class F>
	SignalWrapper& operator+=(F&& func) 
		except_when_m(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		Base::connect(makeFunctor<ftype>(forward_m(func)));
		return *this;
	}
	/*
		\brief	去除与所有与指定的函数对象相等的槽.
		\param	func任意的函数对象,支持可选参数
				(无视顺序,但要求去除cv后必须类型一致,具体见functor)
				必须要与添加时的类型一致才可能成功删除.
		\return	返回对象自身,从而支持链式调用如
				sig -= f1
					-= f2;
		\note	使用者必须当心链式调用时的参数求值顺序不确定
				由于主流信号槽设施都是一次删除所有相等的slot,
				所以此处也如此,如果要仅删除一个则应使用disconnect_one
	*/
	template<class F>
	SignalWrapper& operator-=(F&& func)
		except_when_m(std::declval<Base&>().disconnect(makeFunctor<ftype>(forward_m(func))))
	{
		Base::disconnect(makeFunctor<ftype>(forward_m(func)));
		return *this;
	}

	/*
		\brief	 去除与所有与指定的函数对象相等的槽.
		\note	直接用functor进行包装,从而与connect时的参数类型相称,
				对于不提供等号比较的类型(比如lambda),直接比较类型.
				对于成员函数可以直接 connect(obj,&Obj::mem_func);
	*/
	template<class... Fs>
	decltype(auto) disconnect(Fs&&... func)
		except_when_m(std::declval<Base&>().disconnect(makeFunctor<ftype>(forward_m(func)...)))
	{
		return Base::disconnect(makeFunctor<ftype>(forward_m(func)...));
	}
	template<class... Fs>
	decltype(auto) disconnect_one(Fs&&... func)
		except_when_m(std::declval<Base&>().disconnect_one(makeFunctor<ftype>(forward_m(func)...)))
	{
		return Base::disconnect_one(makeFunctor<ftype>(forward_m(func)...));
	}


	/*
		\brief	添加回调
		\note	直接用functor进行包装,从而提供可选参数支持,
				对于成员函数可以直接 connect(obj,&Obj::mem_func);
	*/
	template<class... Fs>
	decltype(auto) connect(Fs&&... func) 
		except_when_m(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func)...)))
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
		except_when_m(std::declval<Base&>().disconnect_all())
	{
		return Base::disconnect_all();
	}
};

}//namespace Talg

#include "undef_macro.h"

#endif // !SIGNAL_WRAPPER_H_INCLUDED

