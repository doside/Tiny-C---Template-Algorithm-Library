	

//已弃坑

#include "ctstring.h"
#include "exString.h"
#include "test_suits.h"


using namespace Talg;

namespace {
	void test() {
		//constexpr auto res=append(ctString<4>{"abc"},ctString<4>{"abc"});
		constexpr ctString<7> res2{ "abcabc" };
		//TestConstexpr<
		//	std::make_tuple(1, 2, 3) == std::make_tuple(1, 2, 3)
		//	//,res2==res2
		//	,(res.size()>0)
		//	//,res[0]=='a'
		//	//,res==res
		//	//,res2==res
		//>{};
		////constexpr auto res2 = std::array<int, 3>{1, 2, 3};
		static_assert(res2[6]==0, "");//MSVC死活不把这个res当constexpr
		//static_assert(res[0], "");//MSVC死活不把这个res当constexpr
		//static_assert(exStringView{res}[0] == 'a', "");
		//ctString<8> a{"abc", "cde"};
		constexpr ctArray<char, 5> val{ 'a','b','c','d','e' };
		constexpr ctArray<char, 5> val2{ 'a','b','c' };
		constexpr ctArray<char, 5> val3{ 'a','b','c','\0','\0' };
		static_assert(val != val2, "");
		static_assert(val2 == val3, "");
		static_assert(val[0] == val2[0], "");
		static_assert(val[1] == val2[1], "");
		static_assert(val[2] == val2[2], "");
		static_assert(val[3] != val2[3], "");
		static_assert(val[4] != val2[4], "");

		static_assert(val3[0] == val2[0], "");
		static_assert(val3[1] == val2[1], "");
		static_assert(val3[2] == val2[2], "");
		static_assert(val3[3] == val2[3], "");
		static_assert(val3[4] == val2[4], "");

		static_assert(val[0] == val3[0], "");
		static_assert(val[1] == val3[1], "");
		static_assert(val[2] == val3[2], "");
		static_assert(val[3] != val3[3], "");
		static_assert(val[4] != val3[4], "");

		constexpr ctArray<char, 6> val4(val3,'d',std::make_index_sequence<4>());
		static_assert(val4[5] == 'd', "");
		constexpr auto val5 = val2.to_base();
		static_assert(val5 == val5, "");
		constexpr auto val6 = AppendCt<5>::call(val, val2);

		static_assert(val6==ctArray<char,10>{
			'a','b','c','d','e',
			'a','b','c','\0','\0'
		}, "");
		static_assert(val6[5] == 'a', "");
		static_assert(val6 == val6, "");
		static_assert(val6==AppendCt<5>::call(val, val3), "");

	}
}
