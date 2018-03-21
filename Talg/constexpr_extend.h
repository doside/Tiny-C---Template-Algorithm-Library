#pragma once
#include <cstddef>
namespace Talg {

	/*!
		\brief	用于让递归的constexpr函子能够尽可能多的求值而不受过浅的递归深度的影响.
				A function which can evaluate constexpr curry functor recursively 
				in a portable way to overcome the limit depth of constexpr function.
		\tparam	\F	函子类型,需具有done方法,以及函数调用返回类型必须是自己.
					functor type,which must has done()->bool method indicate if it finished 
					and a operator() overload which is used to evaluate.
				\N	最大深度,也即函数最多被调用2^N-1次,默认为15因为MSVC的 大概 限制即是如此.
					max recursive limit,default is N means evaluate functor at most 2^N-1 times.
		\param \func the functor.	\depth recursive depth.
		\return the functor.
		\note	除了递归深度之外,某些编译器对constexpr有最大求值数量的限制(尽管该限制一般远大于递归深度限制),
				所以本质上还是会遇到编译出错的情况,应该尽量保守的估计N的值.
				当functor的done方法返回true时停止求值,也即尽早退出递归.
				在我们测试过的所有编译器上N可以非常大,但是求值次数本身过大会直接让编译器停止工作,
				这个限制实际上是C++14标准加以限制的(具体数量实现定义).
				一些 粗略 的实验表明在MSVC14大概是24996次,在clang3.8上是2^16 ~ 2^17之间, GCC4.9没有限制?
				因为很难回答到底是多少,所以保守估计是多于2^14次,小于2^15次.因此N设为14
				(从而即使done()全返回false也不会编译错误,取较小的N可以减少编译时间)
				return when functor.done()==true || depth==N;
				N could be very big for all compiler we tested,but we choosed a small number,
				because it is rarely to do much more work in compile-time and even though use a bigger N 
				can not make the limit count of call more than a smaller number than N in some compiler,
				for example,MSVC at most 24996, clang at most 2^16, GCC no limit maybe.
		\todo	example for usage.

	*/

	template<class F,size_t N=14>
	constexpr F trampoline(F&& func, size_t depth = 0) {
		//不可使用std::forward因为它会消耗constexpr的求值次数.
		//使用按引用传递是为了避免某些编译器的bug,如MSVC.
		return depth == N || func.done() ?
			func:
			trampoline(trampoline(func(), depth + 1), depth + 1);
	}


	/*!
		\brief trampoline的语法糖,避免手动写出类型F,又能方便地设置N.
				A grammar sugar for trampoline so that we could not write F explicitly.
		\tparam \T a meta class that has static constexpr value member.e.g.,std::integral_constant<..>
				\F functor type that has done()->bool method and operator()()->F overload.
		\param \func functor \depth use for implement recursive.
		\return the functor.
		\note	\see_also trampoline
	*/
	template<class T,class F>
	constexpr F trampolineN(F&& func, size_t depth = 0) {
		//不可使用std::forward,也不宜直接转调trampoline, 因为它们会消耗constexpr的求值次数或是递归深度.
		//使用按引用传递是为了避免某些编译器的bug,如MSVC.
		return depth == T::value || func.done() ?
			func:
			trampoline<F,T::value>(trampoline<F,T::value>(func(), depth + 1), depth + 1);
	}


}