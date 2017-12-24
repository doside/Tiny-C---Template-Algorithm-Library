#pragma once
#include <vector>

namespace Talg {
	template<class Iter,class T,class Vec=std::vector<typename Iter::value_type>>
	Vec sub_vec(Iter iter,T count) {
		Vec res(iter, std::next(iter,count));
		return res;
	}



}