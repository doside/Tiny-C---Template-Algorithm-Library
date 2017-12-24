#include "ctstring.h"
#include "initlist.h"
#include "exString.h"
#include "functional.h"
#include "fold.h"
#include <array>
#include <stdexcept>

/*
enum Color;
YourHasher hashEnum(Color);
enum Color :int DefAsSuperEnum(ColorEx,
	Green = 1,
	Red = (1 << 2),
	Blue = (1 << 3)
);

Hasher hashEnum(Safe);
struct Safe{
	enum class Rank:char DefSuperEnum(Helper,
		High,
		Low,
		Mid
	);

}


*/


#define super_enum_m(Name,Type,...) 													   \
struct Name{																			   \
	struct Helper {																		   \
		static auto count(){														       \
			using Talg::Tagi;															   \
			using Talg::countParams;													   \
			constexpr Type __VA_ARGS__;													   \
			return Tagi<countParams(Type{},__VA_ARGS__)>{};								   \
		}																				   \
		static constexpr size_t size(){													   \
			return decltype(count())::value;											   \
		}																				   \
		template<size_t...Ids>															   \
		static auto make(std::index_sequence<Ids...>){									   \
			using Talg::Enumerator;														   \
			using Talg::ctArray;														   \
			using Talg::parseEnumValue;													   \
			using underlying_type = typename Type::underlying_type;						   \
			constexpr Type __VA_ARGS__;													   \
			constexpr auto res=parseEnumValue(ctArray<underlying_type, 1>(), __VA_ARGS__); \
																						   \
			using HashType = typename Type::template hash_type<res[Ids]...>;			   \
			return HashType{};															   \
		}																				   \
		static constexpr auto call(){												       \
			return decltype(make(std::make_index_sequence<size()>())){}; \
		}																				   \
	};																					   \
	enum{																				   \
		__VA_ARGS__																		   \
	};																					   \
}														

//super_enum_m(Color, enum{
//	Red = 0,
//	Green,
//	Blue = 1 << (2 * 3 + 4 - (12 - 5))
//});
namespace Talg{
struct A{};

template<class...Ts>
struct LinearHash {

};

template<class T,T...vals>
using LinearHash_t = LinearHash<std::integral_constant<T, vals>...>;

template<class T=int,class Hash=LinearHash<>>
struct Enumerator {
	using underlying_type = T;
	template<T...evs>	//enum_values
	using hash_type = Transform_t< Hash,Seq<std::integral_constant<T,evs>...> >;
	
	bool is_init;
	T val;
	
	constexpr Enumerator(T v):is_init(true),val(v){}
	constexpr Enumerator():is_init(false),val{}{}
	constexpr Enumerator(const Enumerator&) = default;

	//因为Enumerator a=3实际上是直接调用了拷贝构造子而不是赋值,所以此处可以安全地禁用它.避免歧义的可能性.
	Enumerator& operator=(const Enumerator&) = delete; 
	constexpr const Enumerator& operator=(T)const { return *this; }
	constexpr T value(T prev)const noexcept{	//如果没有指定enumerator的值有必要根据前一个的值来计算当前值
		return is_init ? val : prev + 1;
	}
};

template<class U,class T>
constexpr auto parseEnumValueImp(U sum,T prev, Enumerator<T> e) {
	return sum + e.value(prev);
}

template<class U,class T,class...Ts>
constexpr auto parseEnumValueImp(U sum,T prev, Enumerator<T> e,Ts...args) {
	return parseEnumValueImp(sum + e.value(prev),  prev+1,  args...);
}

template<class U,class T,class...Ts>
constexpr auto parseEnumValue(U sum,Enumerator<T> e,Ts...args) {
	return parseEnumValueImp(sum+e.value(0), e.value(0),  args...);
}

struct StrPosPair {
	const char* start=nullptr;
	const char* end=nullptr;
	constexpr StrPosPair() = default;
	constexpr StrPosPair(const char* first,const char* last=nullptr)noexcept:start(first),end(last){}
	constexpr bool is_start()const { return start == nullptr; }
};




template<class T,T val,T...vals>
struct ctSwitch {
	template<class D,class U,class...Us>
	static constexpr auto select(const T& obj,const D& default_res,const U& res,const Us&...others){
		return obj == val ? res : select(obj,default_res,others...);
	}
	template<class D,class U>
	static constexpr auto select(const T& obj,const D& default_res,const U& res){
		return obj == val ? res : default_res;
	}
};

/*


struct ParentheMatch {
	size_t small=0;
	size_t big=0;
	size_t bracket=0;
	constexpr ParentheMatch()noexcept = default;
	constexpr ParentheMatch(size_t s,size_t b,size_t bk)noexcept:small(s),big(b),bracket(bk){}
	
	constexpr ParentheMatch match_small()const { 
		return small>0?{ small - 1,big,bracket }:throw std::logic_error{"no matched parenthese."}; 
	}
	constexpr ParentheMatch match_big()const { 
		return big>0?{ small,big-1,bracket }:throw std::logic_error{"no matched big parenthese."}; 
	}
	constexpr ParentheMatch match_bracket()const { 
		return bracket>0?{ small,big,bracket-1 }:*this; 
	}
	constexpr ParentheMatch found_small()const { return{ small + 1,big,bracket }; }
	constexpr ParentheMatch found_big()const { return{ small,big+1,bracket }; }
	constexpr ParentheMatch found_bracket()const { return{ small,big,bracket+1 }; }
	constexpr bool is_matched()const noexcept {
		return small == 0 && big == 0 && bracket == 0;
	}
};








template<class T>
constexpr auto parseEnumName0(const T& sum,const char* str,size_t cur,
	const ParentheMatch& matches,const StrPosPair& res) {
	is_digit(*str) ? 
		throw std::logic_error("ileagal identifier:enum name has number prefix.");
		parseEnumName(sum,str+1,cur+1,matches,StrPosPair{0});
}

template<class T>
constexpr auto parseEnumName(const T& sum,const char* str,size_t cur,
	const ParentheMatch& matches,const StrPosPair& res) {
	res.is_start() ?
		is_digit(*str)?
			throw std::logic_error("ileagal identifier:enum name has number prefix.")  //todo:detect less or greater operator
			:is_identifier(*str)?
				parseEnumName(sum,str+1,cur+1,matches,res):
				(*str=='='||*str==',' ?
					(matches.is_matched()?
						parseEnumName(sum+StrPosPair{res.start,cur},str+1,cur+1,matches,StrPosPair{})
						parseEnumName(sum,str+1,cur+1,matches,res)):
					parseEnumName(sum,str+1,cur+1,
						ctSwitch<char, '<', '(', '{', '}', ')', '>'>::select(*str,
							matches,
							matches.match_bracket(),
							matches.match_small(),
							matches.match_big(),
							matches.found_big(),
							matches.found_small(),
							matches.found_bracket()
						),res))

			
					


	return ;
}*/

////precondition: 已经找到第一个字符的所在
//template<size_t N,size_t M>
//void findEnumStringEnd(const ctString<N>& str, size_t cur,
//						size_t res_beg,const ctArray<StrPosPair,M>& res) {
//	return isalpha(str[cur])?
//				parseEnumString(str,cur+1,res_beg):
//				
//
//}

//
//struct Color {
//	struct Helper {
//		auto count()const{
//			constexpr Enumerator<> Red = 0, Green, Blue=1;
//			constexpr auto res=parseEnumValue(ctArray<int, 1>(), Red = 0, Green, Blue = 1);
//			return res;
//		}
//		template<size_t...Ids>
//		auto f(IdSeq<Ids...>)const{
//			constexpr Enumerator<> Red = 0, Green, Blue=1;
//			constexpr auto res=parseEnumValue(ctArray<int, 1>(), Red = 0, Green, Blue = 1);
//			return std::integer_sequence<int,res[Ids]...>{};
//		}
//		auto call()const {
//			return f(std::make_index_sequence<decltype(count())::length>());
//		}
//	};
//	enum {
//		Red = 0,
//		Green,
//		Blue = 1// << (2 * 3 + 4 - (12 - 5))
//	};
//	
//
//};
constexpr bool is_identifier(char c) {
	return is_digit(c) || is_alpha(c) || c == '_';
}
template<class T,class U>
constexpr bool equal_any_of(const T& lhs,const U& array) {
	return find(array, lhs)!=no_index;
}

#if 0
template<size_t N> //有多少enumerator
struct EnumParser {
	enum TokenType{
		Ename,
		Literal,
		AccessOp,
		LParatheses,
		Expr,
		VarExpr,
		Unknown
	};

	ctArray<StrPosPair, N> sum;
	const char* cur=nullptr;
	const char* next=nullptr;
	const char* res_start=nullptr;
	size_t pc = 0;
	size_t bkc = 0;
	size_t brc = 0;
	size_t sc = 0;	//count of () <> {} []
	TokenType prev_token_type = Unknown;
	
	constexpr EnumParser(const ctArray<StrPosPair,N>& s):sum(s){}
	constexpr EnumParser(
		const ctArray<StrPosPair,N>& s,
		const char* cur_,
		const char* next_,
		const char* res_start_,
		size_t pc_,
		size_t bkc_,
		size_t brc_,
		size_t sc_,	
		TokenType prev_token_type_ 
	):sum(s),cur(cur_),next(next_),res_start(res_start_),
	  pc(pc_),bkc(bkc_),brc(brc_),sc(sc_),prev_token_type(prev_token_type_){}
	
	constexpr EnumParser parseNext()const {
		return EnumParser{ s,next+1,next + 2 ,res_start,pc,bkc,brc,sc,prev_token_type };
	}
	constexpr EnumParser foundAssign()const {
		return EnumParser{ s };//todo fix
	}
	constexpr EnumParser foundBrack()const {
		return EnumParser{ s };//todo fix
	}
	constexpr EnumParser deBrack()const {
		return EnumParser{ s };//todo fix
	}
	constexpr EnumParser foundSubscript()const {
		return EnumParser{ s };//todo fix
	}
	constexpr EnumParser foundBrace()const {
		return EnumParser{ s };//todo fix
	}
	constexpr EnumParser foundComma()const {
		return EnumParser{ s };//todo fix
	}
	constexpr auto operator()()const {
		return  (*next==0?
					(prev_token_type==Ename? 
						EnumParser{sum+StrPosPair{res_start,next}}:
						EnumParser{sum})
				(*cur=='('||prev_token_type==LParatheses?
					fastSkip():
				(*next=='='?
					(equal_any_of(*cur,makeCtString("+-*/%!=&|^"))?
						parseNext():
						foundAssign())
				(*cur=='<'?
					(*next=='<'||prev_token_type==Literal?
						parseNext():
						foundBrack):
				(*cur=='>'?
					(prev_token_type==AccessOp|| is_identifier(*next)?
						parseNext():
						deBrack()):
				(*cur=='['?
					foundSubscript():
				(*cur=='{'?
					foundBrace():
				(*cur==','?
					foundComma():
				))))))));

	}
};
#endif


}

super_enum_m(Color, Talg::Enumerator<>,
	Red = 0,
	Green,
	Blue = 1
);

/*
	struct Hash2{};
	enum Rank{};
	struct Helper2 {
		template<class T> //利用模板来延迟名称查找的时机
		auto make(T&& arg) {
			using H = decltype(hash(arg));
			return H{};
		}
	};
	static Hash2 hash(Rank);
*/




namespace {
	using namespace Talg;

	void test() {
		//Helper2 hhhh;
		//hhhh.make(Rank{});
		using res_type = decltype(Color::Helper::call());
		static_assert(std::is_same<
			res_type, 
			LinearHash_t<int,0, Color::Red, Color::Green, Color::Blue>
		>::value, "");
		//static_assert(res == ctArray<int, 4>({0, Color::Red,Color::Green,Color::Blue }), "");

		//constexpr ParentheMatch res=parseEnumName("kasd", ParentheMatch{});

	}




}
