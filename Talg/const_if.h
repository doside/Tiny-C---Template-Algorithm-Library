#pragma once
#include <type_traits>
#include <Talg/basic_macro_impl.h>
namespace Talg {

	/*!
		\brief	用于编译期作分支选择.
				This class is used for compile-time selection.
		\vparam	\cond编译期条件,条件为真时执行第一个函数对象,否则第二个函数对象被执行.
				\cond a compile-time condition.
				when cond==true used the first callable object,
				otherwise used the second callable object,
				in all cases, the remaining parameters are forwarded to 
				the callable object be called.
		\note	可以设计为可以传递参数,因为某些场合不便使用lambda传递状态甚至不能用lambda.
				We designed this class could be pass parameters to the callable object 
				so that we can avoid usage of captured lambda expression in some case
				(it could improve little performence.),what's more, in some case,we
				even can not use lambda at all, if we could not pass parameter within 
				this class,it would be so hard to use that we had to write some function
				object class by hand.
		\example	ConstIf<cond>(than_do,else_do)
					等价于
					constexpr if(cond){
						than_do();
					}else{
						else_do();
					}
		\example	ConstIf<cond>(than_do,else_do,params...)
					等价于
					constexpr if(cond){
						than_do(params...);
					}else{
						else_do(params...);
					}

	*/
	template<bool cond>
	struct ConstIf {
		template<class Than_f,class Else_f,class...Ts>
		constexpr decltype(auto) operator()(Than_f&& func, Else_f&&, Ts&&...args)const
		except_when_m(forward_m(func)(forward_m(args)...))
		{
			return forward_m(func)(forward_m(args)...);
		}
	};


	/*!
		\brief	特化,当条件为假时执行第二个函数对象.
				specialization for the case when cond is false
	*/
	template<>
	struct ConstIf<false>{
		template<class Than_f,class Else_f,class...Ts>
		constexpr decltype(auto) operator()(Than_f&& , Else_f&& func, Ts&&...args)const
		except_when_m(forward_m(func)(forward_m(args)...))
		{
			return forward_m(func)(forward_m(args)...);
		}
	};


	/*!
		\brief 当cond真时执行第一个函数对象否则执行第二个函数对象,
				剩下的参数会统统传递给被执行的函数对象
				if cond is true than use the first parameter as the callable object,
				else used the second parameter as the callable object,
				finally call the callable object with remaining parameters.
		\param	\args... 
					args[0]:than_do, 
					args[1]:else_do, 
					args[2...]:parameters which be forwarded to args[0] or args[1] 
		\return	args[0](args[2...]) when cond is true,
				args[1](args[2...]) otherwise.
		\note	为求简化,当前直接假定了所有情况下被调用的函数对象的参数以及返回类型相通.
				对于复杂的情况参见apply.h
		\see_also	ConstIf	
		\see_also	Talg/apply.h
	*/
	template<bool cond,class...Ts>
	constexpr decltype(auto) constIf(Ts&&...args)
	except_when_m(ConstIf<cond>{}(forward_m(args)...))
	{
		return ConstIf<cond>{}(forward_m(args)...);
	}





}
#include <Talg/undef_macro.h>


