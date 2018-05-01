#ifndef CALLBLE_TRAITS_H_INCLUDED
#define CALLBLE_TRAITS_H_INCLUDED

#include "seqop.h"
#include "invoke_impl.h"
#include "select_type.h"

#include "strip_qualifier.h"
#include "has_member.h"
#include "basic_macro_impl.h"

namespace Talg{
struct NoCallableTraits{};

template<class F,bool = hasOpCall<F>::value>
struct DeduceCallParam{ 
	using type = NoCallableTraits;
};
template<class F>
struct DeduceCallParam<F,true>{
	using type = RemoveCvrp<decltype(&std::remove_reference_t<F>::operator())>;
};

template<class F>
struct CallableTraits:
	std::conditional_t<std::is_function<F>::value,
		CallableTraits<RemoveCvrp<F>>,
		CallableTraits<typename DeduceCallParam<F>::type>
		//CallableTraits<RemoveCvrp<decltype(& std::remove_reference_t<F>::operator())>>
	> 
{ };
template<>
struct CallableTraits<NoCallableTraits>{};
template<class R ,class...Ts>
struct CallableTraits<R(Ts...)>:std::integral_constant<size_t,sizeof...(Ts)> {
	using arg_type = Seq<Ts...>;
	using ret_type = R;
};
template<class R ,class...Ts>
struct CallableTraits<R(*)(Ts...)>:std::integral_constant<size_t,sizeof...(Ts)> {
	using arg_type = Seq<Ts...>;
	using ret_type = R;
};
template<class R, class...Ts>
struct CallableTraits<std::function<R(Ts...)>> : CallableTraits<R(Ts...)>{};

template<class Obj,class R,class...Ts>
struct CallableTraits<R(Obj::*)(Ts...)>:CallableTraits<R(Ts...)>{};


template<class T>
struct isNonOverloadFunctor {
	template<class U>
	static constexpr bool detect(typename CallableTraits<U>::arg_type* ) { return true; }
	template<class U>
	static constexpr bool detect(...) { return false; }
	static constexpr bool value = detect<T>(nullptr);
};



/*!
	\brief	采用链式继承来从F(T0,T1,...Tn)构造出一排重载
			F(T0) 
			F(T0,T1) 
			F(T0,T1,T2)
			.....
			F(T0,T1,...,Tn)
			参数个数最多的重载在派生链的最末端,从而被最优先选择.
	\tparam	Ts...所有可能的参数类型
*/
template<class...Ts>struct LongFirstCall;



/*!
	\brief	采用链式继承来从F(T0,T1,...Tn)构造出一排重载
			F(T0) F(T0,T1) F(T0,T1,T2).....F(T0,T1,...,Tn)
			当前偏特化构造重载候选F(T<0>...T<k>),并且让基类继续构造重载候选F(T<0>...T<k-1>)
	\tparam	Ts...当前的重载的参数 
*/
template<class...Ts>
struct LongFirstCall<Seq<Ts...>> :public LongFirstCall<PopBack_s<Seq<Ts...>>> {
	using LongFirstCall<PopBack_s<Seq<Ts...>>>::pcall;
	using self = LongFirstCall<Seq<Ts...>>;

	//Us&&...的目的是让所有pcall都能以同样的方式调用
	//self* 是为了利用派生类到基类的转换顺序来逐个sfinae掉不合法的调用
	template<class F, class...Us>
	auto pcall(self*, F&& f, Ts&&...args, Us&&...)
		->Seq< decltype(forward_m(f)(forward_m(args)...)), Ts...>;
	//返回的类型为 Seq<返回值类型, 参数类型>
	//若这调用不合法那么一定会Substitution Failure，然后把指针的类型自动转换为基类指针
	//todo: fix: vs2015 update2之前是无法在decltype内部使用pack expansion 的
};

/*!
	\brief	采用链式继承来从F(T0,T1,...Tn)构造出一排重载
			F(T0) F(T0,T1) F(T0,T1,T2).....F(T0,T1,...,Tn)
			当前偏特化构造最后一个重载候选:F(),从而终止递归.
*/
template<>
struct LongFirstCall<Seq<>> {
	using self = LongFirstCall<Seq<>>;
	template<class F, class...Us>
	auto pcall(self*, F&& f, Us&&...)->Seq<decltype(f())>;	
};


struct LongParser
{
	template<class F, class...Ts>
	auto operator()(F&& f, Ts&&...args)
	->decltype(
			LongFirstCall<Seq<Ts...>>{}.pcall(
				(LongFirstCall<Seq<Ts...>>*)nullptr,
				forward_m(f), forward_m(args)...
			)
	  );

	//template<class F,class...Us>
	//Seq<> operator()(F&& f,EatParam,Us&&...);	//用...的话会使得某些编译器引发意外的警告(如GCC). 
	//
	//template<class F>
	//Seq<> operator()(F&& f,EatParam);	//用...的话会使得某些编译器引发意外的警告(如GCC). 
};


/*!
	\brief	采用链式继承来从F(T0,T1,...Tn)构造出一排重载
			F(T0) 
			F(T0,T1) 
			F(T0,T1,T2)
			.....
			F(T0,T1,...,Tn)
			参数个数最少的重载在派生链的最末端,从而被最优先选择.
	\tparam	Ts...所有可能的参数类型
*/
template<class...Ts>struct ShortFirstCall;

/*!
	\brief	采用链式继承来从F(T0,T1,...Tn)构造出一排重载
			F(T0) F(T0,T1) F(T0,T1,T2).....F(T0,T1,...,Tn)
			当前偏特化构造重载候选F(Ts...),并且让基类继续构造重载候选F(Ts...,U)
	\tparam	Ts...当前的(也是直接基类的)重载的参数	
	\tparam	U基类的重载的最后一个参数
	\tparam	Us...剩下的参数,需要往上传递给其他基类.
*/
template<class...Ts, class U, class...Us>
struct ShortFirstCall<Seq<Ts...>, U, Us...>
	:ShortFirstCall< Seq<Ts..., U>, Us...>
{
	using ShortFirstCall< Seq<Ts..., U>, Us...>::pcall;
	using self = ShortFirstCall<Seq<Ts...>, U, Us...>;

	/*!
		\brief	重载候选
		\tparam	Ps...被忽略的其他参数类型
		\tparam	Ts...被使用的参数类型
		\tparam	F 函子类型
		\param	self* 用于产生重载优先级,因为重载的选择优先使用派生类
		\param	f 被调用的函
		\param	args...参数
		\return	用于SFINAE.如果函子f可以以参数args...调用的话,
				就把参数类型Ts...包装成Seq<R,Ts...>并返回,
				其中R是f的返回类型.
		\note	这个函数是不加实现的,主要用于探测重载.
	*/
	template<class F, class...Ps>
	auto pcall(self*, F&& f, Ts&&...args, Ps&&...)
		->Seq<decltype(forward_m(f)(forward_m(args)...)), Ts...>;
};


/*!
	\brief	采用链式继承来从F(T0,T1,...Tn)构造出一排重载
			F(T0) F(T0,T1) F(T0,T1,T2).....F(T0,T1,...,Tn)
			当前偏特化构造最后一个重载候选,F(T0,...,Tn),从而终止递归.
	\tparam	Ts...当前的重载的参数	
*/
template<class...Ts>
struct ShortFirstCall<Seq<Ts...>> {
	using self = ShortFirstCall<Seq<Ts...>>;

	/*!
		\brief	重载候选
		\tparam	Ps...被忽略的其他参数类型
		\tparam	Ts...被使用的参数类型
		\tparam	F 函子类型
		\param	self* 用于产生重载优先级,因为重载的选择优先使用派生类
		\param	f 被调用的函
		\param	args...参数
		\return	用于SFINAE.如果函子f可以以参数args...调用的话,
				就把参数类型Ts...包装成Seq<R,Ts...>并返回,
				其中R是f的返回类型.
		\note	这个函数是不加实现的,主要用于探测重载.
				这里仍然有Ps...是因为Ts...可以是Ps...的真子集.
	*/
	template<class F, class...Ps>
	auto pcall(self*, F&& f, Ts&&...args, Ps&&...)
		->Seq<decltype(forward_m(f)(forward_m(args)...)), Ts...>;
};



/*!
	\brief	用于启动ShortFirstCall来进行重载选择,以参数最短优先.
	\see	ShortFirstCall
*/
struct ShortParser
{
	template<class F, class First, class...Ts>
	auto operator()(F&& f, First&& arg0, Ts&&...args)
		-> decltype(
			ShortFirstCall<Seq<First>, Ts...>{}.pcall(
			(ShortFirstCall<Seq<First>, Ts...>*)nullptr,
				forward_m(f), forward_m(arg0), forward_m(args)...
			));
	//template<class F>
	//Seq<> operator()(F&&...);
};


template<class...Ts>
constexpr decltype(auto) tryApplyShort(Ts&&...args) 
except_when_m(Transform<GetImp, 
		PopFront_s<decltype(ShortParser{}(forward_m(args)...))>
	>::applyFrontImp(forward_m(args)...)
)
{
	using ParamSeq = PopFront_s<decltype(ShortParser{}(forward_m(args)...))>;
	return Transform<GetImp, ParamSeq>::applyFrontImp(forward_m(args)...);
}

template<class...Ts>
constexpr decltype(auto) tryApplyLong(Ts&&...args) 
except_when_m(Transform<GetImp, 
		PopFront_s<decltype(LongParser{}(forward_m(args)...))>
	>::applyFrontImp(forward_m(args)...)
)
{
	using ParamSeq = PopFront_s<decltype(LongParser{}(forward_m(args)...))> ;
	return Transform<GetImp, ParamSeq>::applyFrontImp(forward_m(args)...);
}

template<class F, class...Ts>
void mapAny(F&& func, Ts&&...args);


template<class...Ts>
struct RepeatImp {
	template<class F>
	constexpr static decltype(auto) repeat(F&& func, Ts&&...args){
		return forward_m(func)(forward_m(args)...);
	}
	template<class F, class...Us>
	constexpr static decltype(auto) repeat(F&& func, Ts&&...args, Us&&...last){
		return (void)forward_m(func)(forward_m(args)...),
		Transform< Talg::RepeatImp, Before_s<sizeof...(Ts), Seq<Us...>> >
			::repeat(forward_m(func), forward_m(last)...);
	}

	template<class F, class...Us>
	static void mapAny(F&& func, Ts&&...args, Us&&...last){
		forward_m(func)(forward_m(args)...);
		mapAny(forward_m(func), forward_m(last)...);
	}
	template<class F, class...Us>
	static void mapAny(F&& func, Ts&&...args){
		forward_m(func)(forward_m(args)...);
	}
};


template<size_t n, class F, class...Ts>
constexpr decltype(auto) repeat(F&& func, Ts&&... args) {
	using begin = Before_s<n, Seq<Ts...>>;
	static_assert(sizeof...(args) % n == 0, "m args should be divide into m/n group");
	return Transform<RepeatImp, begin>::repeat(forward_m(func), forward_m(args)...);
}

template<class F, class...Ts>
void mapAny(F&& func, Ts&&...args) {
	using param_parser = decltype(ShortParser{}(forward_m(func), forward_m(args)...));
	constexpr size_t param_length = SeqSize<param_parser>::value - 1;
	using begin = Before_s<param_length, Seq<Ts...>>;

	return Transform<RepeatImp, begin>::mapAny(forward_m(func), forward_m(args)...);
}



}//namespace Talg

#include "undef_macro.h"

#endif // !CALLBLE_TRAITS_H_INCLUDED

