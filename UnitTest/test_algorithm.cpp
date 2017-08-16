#include <doctest/doctest.h>
#include <algorithm.h>
#include <numeric.h>
#include <map>
using namespace Talg;

TEST_CASE("Set or Add Key") {
	std::map<int, int> table;
	CHECK(addKey(table, 0, 1));
	CHECK(table[0] == 1);
	CHECK(addKey(table, 0, 100)==false);
	CHECK(table[0] == 1);
	CHECK(setKey(table, 0, 200));
	CHECK(table[0] == 200);
	CHECK(setKey(table, 10, 200) == false);
	CHECK(toKey(table, 10, 200) == 200);
	CHECK(toKey(table, 0, 30) == 200);

}

TEST_CASE("Talg::inner_product") {




}