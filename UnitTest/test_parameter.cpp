#include "lazy_add.h"
#include <parameter.h>
namespace {
	UseName;
	template<class...Ts>
	void test(const Ts&... args){
		InitList<char,Ts...> list(args...);
		constexpr const char a = 'a';
		
		constexpr InitList<char, Ts...> clist(a,a,a); //gcc以及clang都无法通过此行！
		constexpr auto cc = clist.begin();
		constexpr auto cc2 = cc[1];
		for (auto& arg : list) {
			
		}
	}

	void f() {
		test('a','b','c');
	}



}
