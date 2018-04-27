#include <sstream>
#include <iostream>
#include <Talg/core.h>
#include <Talg/basic_macro.h>

#include <Talg/chrono_io.h>
#include <Talg/printer.h>
#include <Talg/basic_macro.h>
#include <doctest/doctest.h>
	//using namespace Talg;

	//template<class Out,class T>
	//auto& print2(Out& out, T&& first) {
	//	using ty = decltype(first);
	//	static_assert(staticCheck<hasOverloadOstream<Out,ty>::value,T>(),"");
	//	return out << forward_m(first);
	//}

TEST_CASE("chrono") {
	using namespace std;
	using Talg::print;
	using Talg::AutoSpace;
	auto pt_h = chrono::high_resolution_clock::now();
	auto pt_steady = chrono::steady_clock::now();
	auto pt_sys = chrono::system_clock::now();
	ostringstream out;
	//static_assert(hasOverloadOstream<decltype(out), decltype(1ns - 2ns)>::value==false, "");
	//using ty = ;
	//staticCheck<hasOverloadOstream<decltype(out), RefForStd<const decltype(1ns - 2ns)&>>::value>();
	//static_assert(hasOverloadOstream<decltype(out), int>::value, "");
	//print(out, 1ns - 2ns);
	auto a = 1ns - 2ns;
	//RefForStd<decltype(1ns - 2ns)&&> r{1ns-2ns};
	//RefForStd<const decltype(1ns - 2ns)&> pp{ r};
	Talg::print(out,AutoSpace{},1ns - 2ns);
	Talg::print(out,1ns, 2us, 3ms, 4s, 5min, 6h,
		std::chrono::steady_clock::now()-pt_steady,
		std::chrono::high_resolution_clock::now()-pt_h,
		std::chrono::system_clock::now()-pt_sys
	);
	CHECK(out.str() == "1ns 2us 3ms 4s 5min 6h ");

}