#include <Talg/exString.h>
#include <Talg/index_seq.h>
#include "test_suits.h"
#include <Talg/callable_traits.h>
#include <tuple>
#include <stdexcept>
using namespace Talg;
template <char...cs>
struct expstr{};
template <char...cs> expstr<cs...> operator "" _test() {
	return expstr<cs...>{};
}

struct constexprLogicError:std::logic_error{
	using logic_error::logic_error;
};

constexpr int constexprAssert(bool res) {
	return res ? 0 : throw constexprLogicError{ "should never be called." };
}

constexpr exStringView operator""_estr(const char* str,std::size_t len)noexcept{
	return {str,len};
}

template<class T,class U>
constexpr bool equalAnyOf(T&& lhs, U&&rhs){
	return forward_m(lhs) == forward_m(rhs);
}

template<class T,class U,class...Ts>
constexpr bool equalAnyOf(T&& lhs,U&&rhs,Ts&&...next){
	return forward_m(lhs) == forward_m(rhs) || equalAnyOf(forward_m(lhs), forward_m(next)...);
}

enum AlignFlag{
	internal='=',
	center='_',
	show_pos='+',
	left='-',
};
struct FillFlag {
	char data;
};
struct WidthFlag{
	int nth;
	int value;
};
struct PrecisionFlag{
	int nth;
	int value;
};
struct ConverTypeFlag{
	int value;
};


#if 0
struct SpecFlagSeq{
	//AlignFlag align;
	//FillFlag fill;
	//WidthFlag width;
	//PrecisionFlag precision;
	//ConverTypeFlag type_flag;
	using size_type = typename exStringView::size_type;
	//size_type beg_, end_,index_;

	constexpr SpecFlagSeq(...){}
};



constexpr size_t findSpec(const exStringView& str, size_t cur=0) {
	return cur+2>str.size()?
			exStringView::npos:
			str[cur]=='%'&&str[cur+1]!='%'?
				cur:
				findSpec(str,cur+2);
}

/*
	\brief	计算有多少个%
*/
constexpr size_t countSpec(const exStringView& str, size_t cur=0,size_t cnt=0) {
	return cur+2>str.size()?
			cnt:
			str[cur]=='%'&&str[cur+1]!='%'?
				countSpec(str,cur,cnt+1):
				countSpec(str,cur+2,cnt);
}





constexpr SpecFlagSeq parseAlign(size_t index, const exStringView& str, size_t beg, size_t cur_pos) {
	return 0;// str[cur_pos]

}





constexpr SpecFlagSeq parseSpecFlag(size_t index,const exStringView& str,size_t beg, size_t cur_pos) {
	return SpecFlagSeq{
		//beg,cur_pos,
		//parseAlign(index,str,beg,cur_pos),
		//parseFill(index,str,beg,cur_pos),
		//parseWidth(index,str,beg,cur_pos),
		//parsePrecision(index,str,beg,cur_pos),
		//parseConverType(index,str,beg,cur_pos)
	};
}

constexpr size_t ignoreOptionFlag(const exStringView& str,size_t cur_pos){
	return  equalAnyOf(str[cur_pos],'+','-','_','=','0',' ','#')?
				ignoreOptionFlag(str,cur_pos+1):
				cur_pos;
}
constexpr size_t ignoreSelect(const exStringView& str, size_t cur_pos) {
	return is_digit(str[cur_pos])?
				ignoreSelect(str,cur_pos+1):
				str[cur_pos]=='$'?
					cur_pos+1:
					cur_pos;
}

constexpr size_t ignoreWidth(const exStringView& str,size_t cur_pos,char state=0){
	return  state==0?
				(str[cur_pos]=='*'?
					ignoreWidth(str,cur_pos+1,'*'): 
					is_digit(str[cur_pos])?
						ignoreWidth(str,cur_pos+1,'1'):
						cur_pos):
			state=='1'?
				(is_digit(str[cur_pos])?
					ignoreWidth(str,cur_pos+1,'1'):
					cur_pos):
			state=='*'?
				(is_digit(str[cur_pos])?
					ignoreWidth(str,cur_pos+1,'@'):
					cur_pos):
			state=='@'?
				(is_digit(str[cur_pos])?
					ignoreWidth(str,cur_pos+1,'@'):
					str[cur_pos]=='$'?
						cur_pos+1:
						constexprAssert(false)):
			constexprAssert(false);	
}
constexpr size_t ignorePrecision(const exStringView& str,size_t cur_pos){
	return str[cur_pos]=='.'?
				ignoreWidth(str,cur_pos+1):
				cur_pos;
}

constexpr size_t ignoreSpecFlag(const exStringView& str, size_t cur_pos) {
	return	ignorePrecision(str, ignoreWidth(str,ignoreOptionFlag(str, ignoreSelect(str, cur_pos))));
}

/*
	\brief	
	\param
	\return
	\note
*/
constexpr SpecFlagSeq parseSpecFlagSeq(size_t index,const exStringView& str,size_t cnt=0,
										size_t cur_pos=0){
	return  //constexprAssert(cur_pos!=exStringView::npos),
			cnt==index?
				parseSpecFlag(index,str,cur_pos,findSpec(str,cur_pos)):
				cnt!=index-1?
					parseSpecFlagSeq(index,str,cnt+1,findSpec(str,cur_pos)):
					parseSpecFlagSeq(index,str,cnt+1,ignoreSpecFlag(str,cur_pos));
			
}


#endif





#if 0
"kisjfkj%"
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
		{
			auto dt = 123._test;
			(void)dt;
		}
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