#include "callable_traits.h"
#include "test_suits.h"
#include <iostream>

struct Printor
{
	char* operator()(const char* s) {
		std::cout << s;
		return nullptr;
	}
	char operator()(char s) {
		std::cout << s;
		return 'a';
	}
	template<class T>
	int operator()(T&& s) {  //T&& 会把const char&&以及const char&的char&&抢掉
		std::cout << forward_m(s);
		return 0;
	}
};


namespace {
	void g() {
		testSame(
			char*,
			decltype(repeat<1>(
				Printor{}, 1, "!"
			)),

			char,
			decltype(Printor{}('a')),
			
			char,
			decltype(repeat<1>(
				Printor{}, 1, "!",'a'
			))
		);
	}
}










