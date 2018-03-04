#include <Talg/fold.h>
#include <Talg/select_type.h>
#include <Talg/functional.h> //for plus minus
#include <Talg/basic_macro.h>
namespace {
	
	
	using namespace Talg;
	static_assert( (plus{}(1, 2)) == 3, "");
	static_assert(foldl(plus{}, 1, 2, 3) ==  1+2+3, "");
	static_assert(foldl(minus{}, 1, 2, 3) == (1-2)-3, "");
	static_assert(foldr(minus{}, 1, 2, 3) == 1-(2-3), "");

	static_assert(nfoldl<2>(plus{}, 1, 2, 3) == 1+2+3, "");
	static_assert(nfoldl<2>(minus{}, 1, 2, 3) == (1-2)-3, "");
	static_assert(nfoldr<2>(minus{}, 1, 2, 3) == 1-(2-3), "");
	
	template<size_t N,class F>
	struct Nop {
		F func;
		constexpr Nop(F f):func(f){}
		template<class...Ts>
		constexpr decltype(auto) operator()(Ts&&...args)const {
			return applyFront<N>(func, forward_m(args)...);
		}
	};
	template<size_t N,class F>
	constexpr auto makeNop(F func) {
		return Nop<N,F>{func};
	}
	
	struct Sum {
		template<class...Ts>
		constexpr auto operator()(Ts...args)const{
			return foldl(plus{}, args...);
		}
	};
	
	struct Minus {
		template<class...Ts>
		constexpr auto operator()(Ts...args)const{
			return foldl(minus{}, args...);
		}
	};
	
	
	template<size_t N,class F,class...Ts>
	constexpr auto testFoldlNop(F&& func,Ts&&...args) {
		return nfoldl<N>(makeNop<N>(forward_m(func)), forward_m(args)...);
	}
	
	void f() {
		static_assert(apply(Seq<>{},Sum{},1,2,3) == 6, "");
		constexpr auto val3 = testFoldlNop<3>(Sum{}, 1, 2, 3, 4, 5);
		static_assert(val3 == (1 + 2 + 3) + 4 + 5, "");
		
		constexpr auto val4 = testFoldlNop<4>(Sum{}, 1, 2, 3, 4, 5, 6, 7, 8, 9,10);
		static_assert(val4 == ((1 + 2 + 3 + 4) + 5 + 6 + 7) + 8 + 9 + 10, "");
		
		constexpr auto val3m = testFoldlNop<3>(Minus{}, 1, 2, 3, 4, 5);
		static_assert(val3m == (1 - 2 - 3) - 4 - 5, "");
		
		constexpr auto val4m = testFoldlNop<4>(Minus{}, 1, 2, 3, 4, 5, 6, 7, 8, 9,10);
		static_assert(val4m == ((1 - 2 - 3 - 4) - 5 - 6 - 7) - 8 - 9 - 10, "");


		
	}
}