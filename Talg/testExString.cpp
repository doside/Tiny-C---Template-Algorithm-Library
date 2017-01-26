#include "exString.h"
#include "index_seq.h"
#include "test_suits.h"
#include "callable_traits.h"


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

		static_assert(exStringView{ "akshdka" }.find("asdjlakjd") == no_index, "");
		static_assert(exStringView{ "akshdka" }.find("shdk") == 2, "");
		static_assert(exStringView{ "akshdka" }.find("shdkae") == no_index, "");
		static_assert(exStringView{ "akshdka" }.find("hdks") == no_index, "");
		static_assert(exStringView{ "aaaaa" }.find("aa") == 0, "");
		static_assert(exStringView{ "aaaaaaaab" }.find("aaab") == 5, "");

		constexpr auto res=append(ctString<4>{"abc"},ctString<4>{"abc"});
		//static_assert(exStringView{res}[0] == 'a', "");
		//ctString<8> a{"abc", "cde"};
	}


}