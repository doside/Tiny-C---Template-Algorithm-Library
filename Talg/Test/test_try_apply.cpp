
#include <Talg/core.h>
#include <Talg/callable_traits.h>
#include <Talg/algorithm.h>
#include <Talg/basic_macro.h>
using namespace Talg;

namespace {

	struct F {
		constexpr double operator()()const{
			return 0.0;
		}
		constexpr char operator()(char)const{ return '0'; }
		constexpr int operator()(int a,int b)const noexcept{ return a+b; }
	};

	void test_param_deduce() {
		constexpr auto res = tryApplyLong(F{}, 1000, 2000);
		static_assert( isAllEqual(res,1000 + 2000,F{}(1000,2000)), "");

		constexpr auto char_value = tryApplyLong(F{}, 'a');
		static_assert(isAllEqual(char_value,F{}('a'),'0'),"");

		constexpr auto fval = tryApplyLong(F{});
		static_assert(isAllEqual(fval,F{}(),0.0),"");


	}




}











#include <Talg/undef_macro.h>

