#include <Talg/core.h>
#include <Talg/default_type.h>
#include <map>
#include <doctest/doctest.h>
using namespace Talg;


namespace {

	

	template<class...Ts>
	struct map {
		using entity=std::map<Ts...>; 
	};
	template<class...Ts>
	using map_alias = std::map<Ts...>;

	template<class A=float,class B=char,class C=double,class D=int>
	struct Test {};

	TEST_CASE("Test for AsAlias"){
		using Alloc=std::allocator<std::pair<const int, int>>;
		As<std::map<int, int, default_t,Alloc>> v;
		v[2] = 3;
		CHECK(v[2] == 3);
		staticCheck<
			std::is_same<
				As<Test<int,default_t,default_t,char>>,
				Test<int,char,double,char>
			>
		>();

		AsAlias<map<int, double, default_t, Alloc>> a;
		As<std::map<int, double, default_t, Alloc>> b;
		staticCheck<std::is_same<decltype(a), std::map<int, double, std::less<int>, Alloc>>>();
		staticCheck<std::is_same<decltype(b), std::map<int, double, std::less<int>, Alloc>>>();
		staticCheck<std::is_same<As<map_alias<double,int>>, std::map<double,int>>>();
		staticCheck<std::is_same<As<std::map<double,int,default_t>>, std::map<double,int>>>();
		staticCheck<std::is_same<As<map_alias<double,int,default_t>>, std::map<double,int>>>();
		//As<map_alias<default_t, int>> vv;
		//int c = a;
		//int b = a;
	}





}