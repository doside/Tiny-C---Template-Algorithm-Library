	

//已弃坑

#include "ctstring.h"
#include "exString.h"
#include "test_suits.h"


using namespace Talg;

namespace {
	void test() {
		constexpr auto res=append(ctString<4>{"abc"},ctString<4>{"abc"});
		constexpr ctString<7> res2{ "abcabc" };
		TestConstexpr<
			std::make_tuple(1, 2, 3) == std::make_tuple(1, 2, 3)
			,res2==res2
			,(res.size()>0)
			//,res[0]=='a'
			//,res==res
			//,res2==res
		>{};
		//constexpr auto res2 = std::array<int, 3>{1, 2, 3};
		//static_assert(res == res, "");//MSVC死活不把这个res当constexpr
		static_assert(exStringView{res}[0] == 'a', "");
		//ctString<8> a{"abc", "cde"};
	}
}
