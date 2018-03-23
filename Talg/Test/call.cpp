#include <Talg/call.h>


namespace {
	using namespace Talg;
	struct A {
		int id;
		constexpr A(int i):id(i){}
		constexpr auto operator()()const{ return id; }
	};
	struct B {
		int id;
		constexpr B(int i):id(i){}
		constexpr auto operator()()const{ return -id; }
	};

	static_assert(forEach(A(1), A(2), A(3)) == 3, "");
	static_assert(forEach(A(1), A(2), A(3), B(1)) == -1, "");
	static_assert(forEach(A(1), A(2), B(1), A(3)) == 3, "");
	static_assert(forEach(A(1), B(1), A(2), A(3)) == 3, "");
	static_assert(forEach(B(1), A(1), A(2), B(1), A(3)) == 3, "");
	  static_assert(forEach(B(1), A(1), A(2), B(1), A(3)) == 3, "");
}

