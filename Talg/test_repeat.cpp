﻿#include "callable_traits.h"
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


template<size_t n>
struct Selector {
	constexpr Selector(){}
	template<class...Ts>
	constexpr auto operator()(Ts&&...args)const noexcept{
		return get<n>(forward_m(args)...);
	}
};

struct testGatherConstexpr {
	constexpr int operator()(int, int, int a)const {
		return a;
	}
};

template<class...Ts>
constexpr decltype(auto) testDeclAuto(Ts&&...args){
	return get<2>(args...);
}


int main()
{
	using namespace std;
	static_assert(Selector<3>{}(1, 2, 3, 4, 5), "");
	static_assert(
		gather<3>(
			Selector<3>{},
			testGatherConstexpr{},
			1, 2, 3,	4, 5, 6,	
			7, 8, 9,	10, 11, 12
			) == 12
		, ""
	);
	auto show=[](auto&&...arg){
		mapAny([](auto e){cout<<e;},arg...);
	};
	auto double_val=[](const auto& arg){ return arg*2; };
	auto sum = [](auto lhs, auto rhs) { return lhs + rhs; };
	gather<1>(show,double_val,0,1,2,3);  //show(double_val(0),double_val(1),double_val(2),double_val(3))
	cout<<endl;
	gather<2>(show,sum,0,1,  2,3);
	applyRag(IdSeq<2, 6>{}, 
		[](auto&&...args) { mapAny([](auto&&arg) {cout << arg; }, args...); },
		0, 1, 2, 3, 4, 5, 6
	);
	cout << endl;
	applyRag(
		IdSeq<2, 5>{},
		[](int a, int b, int c) {cout << a << b << c; }, 
		0, 1, 2, 3, 4, 5, 6
	);
	return 0;
}







