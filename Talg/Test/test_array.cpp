#include <Talg/array.h>
#include <doctest/doctest.h>
#include <Talg/algorithm.h>
#include <Talg/numeric.h>
#include <map>
using namespace Talg;
TEST_CASE("Test Array") {
	Array<int, 8> i8{};
	Array<int, 1> i1{};
	Array<int, 0> i0;
	static_assert(i0.size() == 0, "");
	CHECK_THROWS(i0.at(0));
	CHECK_THROWS(i0.at(1));
	CHECK(i8.empty()==false);
	static_assert(i8.size() == 8 && i1.size() == 1,"");
	CHECK(i1.back() == i1.front());
	CHECK(all_of(i1, predVal(0)));
	for (auto& e : i1) {
		CHECK(e == 0);
	}
	CHECK(all_of(i8, predVal(0)));
	CHECK(all_of(i8, predRef(0)));
	for (auto& e : i8) {
		CHECK(e == 0);
	}
	i8.fill(1);
	CHECK(all_of(i8, predVal(1)));
	CHECK(all_of(i8, predRef(1)));
	i1.assign(i8.begin(), i8.end());
	CHECK(all_of(i1, predRef(1)));
	CHECK(all_of(i1, predVal(1)));

}
