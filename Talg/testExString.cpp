#include "exString.h"
#include "index_seq.h"
#include "test_suits.h"
#include "callable_traits.h"
#include <tuple>


template <char...cs>
struct expstr{};
template <char...cs> expstr<cs...> operator "" _test() {
	return expstr<cs...>{};
}



constexpr exStringView operator""_estr(const char* str,std::size_t len)noexcept{
	return {str,len};
}
#if 0
struct PrintFormater {
	using size_type = typename exStringView::size_type;
	using state = size_type;
	constexpr state fail_case=exStringView::npos;
	enum {
		Str=0,
	};
	exStringView str;
	size_type cur_pos;

	constexpr char getToken()const noexcept{
		return str[cur_pos];
	}
	constexpr char nextToken()noexcept{
		return str[++cur_pos];
	}
	/*
		\brief	计算有多少个conversion specifications
	*/
	constexpr size_type countSpec(size_type count) {
	/*
		note:
		Every value computation and side effect of the first (left) argument of the 
		built-in logical AND operator && and the built-in logical OR operator ||
		is sequenced before every value computation 
		and side effect of the second (right) argument.

		seek has side effect on this.
	*/
		return seek('%')==exStringView::npos
				&& parse()?
				count:
				countSpec(count + 1);
	}

	findUseParamEnd() {

	}

	state isUseParam(char c) {
		return c == '$' ? true : 
				is_digit(c)?
					isUseParam(str,cur_pos+1):fail_case;
	}

	static constexpr bool isAlignmentChar(char c)noexcept{
		return c == '+' || c == '-' || c == '=';
	}

};
#endif



namespace {

	void f() {
		auto dt = 123._test;
		testSame(
			Tagi < "aksjla"_estr .find("jla") > ,
			Tagi<3>
		);
		TestConstexpr<
			is_digit('0'),
			is_digit('1'),
			is_digit('2'),
			is_digit('3'),
			is_digit('4'),
			is_digit('5'),
			is_digit('6'),
			is_digit('7'),
			is_digit('8'),
			is_digit('9'),
			is_alpha('a'),
			is_alpha('b'),
			is_alpha('c'),
			is_alpha('d'),
			is_alpha('e'),
			is_alpha('f'),
			is_alpha('g'),
			is_alpha('h'),
			is_alpha('i'),
			is_alpha('j'),
			is_alpha('k'),
			is_alpha('l'),
			is_alpha('m'),
			is_alpha('n'),
			is_alpha('o'),
			is_alpha('p'),
			is_alpha('q'),
			is_alpha('r'),
			is_alpha('s'),
			is_alpha('t'),
			is_alpha('u'),
			is_alpha('v'),
			is_alpha('w'),
			is_alpha('x'),
			is_alpha('y'),
			is_alpha('z'),
			exStringView{ "akshdka" }.find("asdjlakjd") == no_index,
			exStringView{ "akshdka" }.find("shdk") == 2,
			exStringView{ "akshdka" }.find("shdkae") == no_index,
			exStringView{ "akshdka" }.find("hdks") == no_index,
			exStringView{ "aaaaa" }.find("aa") == 0,
			exStringView{ "aaaaaaaab" }.find("aaab") == 5
		>{};


	}


}