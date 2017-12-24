#pragma once
#include <numeric>
#include <utility>  //for std::forward
#include "iter_range.h"

namespace Talg {
	template<class ForwardRange, class T=typename ForwardRange::value>
	void iota(const ForwardRange& rag, T&& value=T()) {
		std::iota(rag.begin(), rag.end(), std::forward<T>(value));
	}


	template< class InputRange, class T=typename InputRange::value>
	T sum(const InputRange& rag, T&& init=T()) {
		return std::accumulate(rag.begin(), rag.end(), std::forward<T>(init));
	}

	template< class InputRange,class BinaryOp,class T=typename InputRange::value>
	T sum(const InputRange& rag,BinaryOp op, T&& init=T()) {
		return std::accumulate(rag.begin(), rag.end(), std::forward<T>(init),op);
	}


	template< class InputRag1, class InputRag2,
				class T=typename InputRag1::value_type
				//decltype(
				//(*std::declval<InputRag1::value_type&>()) *  (*std::declval<InputRag1::value_type&>())
				//)
    >
	T dot(const InputRag1& lhs, const InputRag2& rhs, T&& value=T()) {
		return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), std::forward<T>(value));
	}

	template<class InputRag1, class InputRag2, class BinaryOp1, class BinaryOp2,
				class T=typename InputRag1::value_type
				//decltype(
				//*std::declval<InputRag1::value_type&>() *  (*std::declval<InputRag1::value_type&>())
				//)
    >
	T dot( const InputRag1& lhs, const InputRag2& rhs,
					 BinaryOp1 op1, BinaryOp2 op2,T&& value=T()){
		return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(),
				std::forward<T>(value), op1, op2);
	}



}
