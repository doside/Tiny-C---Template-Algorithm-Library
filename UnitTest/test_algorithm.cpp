#include <doctest/doctest.h>
#include <algorithm.h>
#include <map>
TEST_CASE("Set or Add Key") {
	using namespace Talg;
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