#include <Talg/ctstring.h>
#include <Talg/initlist.h>
#include <array>
template<class N,class T>
struct TupleIter {

};

namespace {
	using namespace Talg;

	struct A {
		char v;
		constexpr A(const InitList_t<char,5>& val):v(val[4]){}
	};


	void test() {
		constexpr InitList_t<char, 3> a3{ 1,2,3 };
		constexpr InitList_t<char, 3> a2{ 1,2 };
		constexpr InitList_t<char, 3> a1{ 1 };
		constexpr InitList_t<char, 3> a0{ };
		//todo: constexpr InitList_t<char, 3> c{ "ab" };
		constexpr ctArray<char, 3> ar = a3.cast<InitList_t<char, 3>>();
		static_assert(ar[0]==a3[0]&&ar[1]==a3[1]&&ar[2]==a3[2], ""); 
		//zz clang3.8并不支持constexpr std::array::operator[] 
		constexpr A v0{ {} };
		constexpr A v1{ {'a'} };
		constexpr A v2{ {'a','b'} };
		constexpr A v3{ {1,2,3} };
		constexpr A v4{ {1,2,3,4} };
		constexpr A v5{ {1,2,3,4,'a'} };
		static_assert(a3[0] == 1, "");
		static_assert(a3[1] == 2, "");
		static_assert(a3[2] == 3, "");
		static_assert(a2[2] == 0, "");
		static_assert(a0[2] == 0, "");

	}







}
