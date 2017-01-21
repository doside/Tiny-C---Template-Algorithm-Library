#include "exString.h"
#include "index_seq.h"
#include "test_suits.h"
#include "callable_traits.h"


template <char...cs>
struct expstr{};
template <char...cs> expstr<cs...> operator "" _test() {
	return expstr<cs...>{};
}










namespace {

	void f() {
		auto dt = 123._test;
		testSame(
			Tagi < exString{ "aksjla" }.find("jla") > ,
			Tagi<3>
		);
		static_assert(is_digit('0'), "");
		static_assert(is_digit('1'), "");
		static_assert(is_digit('2'), "");
		static_assert(is_digit('3'), "");
		static_assert(is_digit('4'), "");
		static_assert(is_digit('5'), "");
		static_assert(is_digit('6'), "");
		static_assert(is_digit('7'), "");
		static_assert(is_digit('8'), "");
		static_assert(is_digit('9'), "");
		static_assert(is_alpha('a'), "");
		static_assert(is_alpha('b'), "");
		static_assert(is_alpha('c'), "");
		static_assert(is_alpha('d'), "");
		static_assert(is_alpha('e'), "");
		static_assert(is_alpha('f'), "");
		static_assert(is_alpha('g'), "");
		static_assert(is_alpha('h'), "");
		static_assert(is_alpha('i'), "");
		static_assert(is_alpha('j'), "");
		static_assert(is_alpha('k'), "");
		static_assert(is_alpha('l'), "");
		static_assert(is_alpha('m'), "");
		static_assert(is_alpha('n'), "");
		static_assert(is_alpha('o'), "");
		static_assert(is_alpha('p'), "");
		static_assert(is_alpha('q'), "");
		static_assert(is_alpha('r'), "");
		static_assert(is_alpha('s'), "");
		static_assert(is_alpha('t'), "");
		static_assert(is_alpha('u'), "");
		static_assert(is_alpha('v'), "");
		static_assert(is_alpha('w'), "");
		static_assert(is_alpha('x'), "");
		static_assert(is_alpha('y'), "");
		static_assert(is_alpha('z'), "");

		static_assert(exString{ "akshdka" }.find("asdjlakjd") == no_index, "");
		static_assert(exString{ "akshdka" }.find("shdk") == 2, "");
		static_assert(exString{ "akshdka" }.find("shdkae") == no_index, "");
		static_assert(exString{ "akshdka" }.find("hdks") == no_index, "");
		static_assert(exString{ "aaaaa" }.find("aa") == 0, "");
		static_assert(exString{ "aaaaaaaab" }.find("aaab") == 5, "");


	}


}