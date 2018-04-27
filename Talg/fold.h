#pragma once
#include "seqop.h"
#include "basic_macro_impl.h"

namespace Talg {
	template<class F,class T,class U>
	constexpr decltype(auto) foldr(F&& func,T&& lhs,U&& rhs) {
		return forward_m(func)(forward_m(lhs),forward_m(rhs));
	}
	

	/*!
		\brief	右fold,从最后的参数开始计算一直折叠到第一个
		\return	func(arg,func(args[0](func(args[1],func(args[2]...))))
		\example	foldr(minus{},1,2,3,4)==1-(2-(3-4))
	*/
	template<class F,class T,class...Ts>
	constexpr decltype(auto) foldr(F&& func,T&&arg,Ts&&...args) {
		return forward_m(func)(
			forward_m(arg), 
			foldr(forward_m(func), forward_m(args)...)
		);
	}

	
	
	template<class F,class T,class U>
	constexpr decltype(auto) foldl(F&& func,T&& lhs,U&& rhs) {
		return forward_m(func)(forward_m(lhs),forward_m(rhs));
	}

	/*!
		\brief	左fold,从第一个参数开始计算一直折叠到最后一个
		\return	func(func(func(arg,args[0]),args[1])...,args[n-1])
		\example	foldl(+,1,2,3,4)==(((1+2)+3)+4)
	*/
	template<class F,class T,class U,class...Ts>
	constexpr decltype(auto) foldl(F&& func,T&&lhs,U&& rhs,Ts&&...args) {
		return foldl(
			forward_m(func), 
			forward_m(func)(forward_m(lhs), forward_m(rhs)),
			forward_m(args)...
		);
	}

	template<class F,class T,class U,class...Ts>
	constexpr decltype(auto) foldLL(F&& func,T&&res,U&& rhs,Ts&&...args) {
		return foldLL(
			forward_m(func), 
			forward_m(func)(forward_m(res), forward_m(rhs),forward_m(args)...),
			forward_m(args)...
		);
	}
	template<class F,class T,class U>
	constexpr decltype(auto) foldLL(F&& func,T&& res) {
		return forward_m(res);
	}


	template<class...Ts>
	struct nFoldImp {
		static constexpr size_t N = sizeof...(Ts);
		
		template<class F,class...Us>
		static constexpr decltype(auto) foldl(F&& func,Ts&&...args,Us&&...others) {
			return Transform_t<
				nFoldImp,
				PushFront_s<
					decltype(forward_m(func)(forward_m(args)...)),
					Before_s<N-1,Seq<Us...>>
				>
			>::foldl(
				forward_m(func), forward_m(func)(forward_m(args)...), forward_m(others)...
			);
		}
		
		template<class F,class...Us>
		static constexpr decltype(auto) foldr(F&& func, Ts&&...args, Us&&...others) {
			return forward_m(func)(
				forward_m(args)..., 
				Transform_t<
					nFoldImp,
					Before_s<N,Seq<Us...>>
				>::foldr(forward_m(func),forward_m(others)...)
			);
		}

		template<class F,class...Us,class T>
		static constexpr decltype(auto) foldr(F&& func, Ts&&...args,T&& last) {
			return forward_m(func)(forward_m(args)...,forward_m(last));
		}
		template<class F,class...Us>
		static constexpr decltype(auto) foldl(F&& func, Ts&&...args) {
			return forward_m(func)(forward_m(args)...);
		}
	};

	template<size_t N,class F,class...Ts>
	constexpr decltype(auto) nfoldl(F&& func,Ts&&...args) {
		static_assert(N > 1, "there is no meaning for fold<1> or fold<0>");
		static_assert((sizeof...(args)-1) % (N-1) == 0, "args count is not match for such kind of fold");
		return Transform<nFoldImp, 
			Before_s<N, Seq<Ts...>> 
		>::foldl(
			forward_m(func), forward_m(args)...
		);
	}
	template<size_t N,class F,class...Ts>
	constexpr decltype(auto) nfoldr(F&& func,Ts&&...args) {
		static_assert(N > 1, "there is no meaning for fold<1> or fold<0>");
		static_assert((sizeof...(args)-1) % (N-1) == 0, "args count is not match for such kind of fold");
		return Transform<nFoldImp, 
			Before_s<N-1, Seq<Ts...>> 
		>::foldr(
			forward_m(func), forward_m(args)...
		);
	}
}

#include "undef_macro.h"


