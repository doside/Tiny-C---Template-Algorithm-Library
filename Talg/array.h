#pragma once
#include "algorithm.h"
namespace Talg {

	template<class T,size_t N>
	struct Array {
		T data[N];
		constexpr Array() = default;
		template<class Iter>
		Array(Iter beg, Iter end) {
			std::copy(beg, end, std::begin(data));
		}
		Array(const T& val) {
			fill(data,N,val);
		}
	};


}