#include <doctest/doctest.h>
#include <Talg/algorithm.h>
#include <Talg/numeric.h>
#include <map>
#include <Talg/basic_macro.h>
using namespace Talg;
struct A {
	bool c1 = false;
	A(int) { throw 1; }
	A(){}
	A(char):c1(true){}
	bool operator==(const A& rhs)const { 
		return c1==rhs.c1; 
	}
};

TEST_CASE("Set or Add Key") {
	using std::map;
	map<int, int> table;
	auto basic_try_insert_test = [](auto& table,auto&& key,auto&&...args) {
		CHECK(tryInsertKey(table,forward_m(key),forward_m(args)...));
		CHECK(hasKey(table,forward_m(key)));
		CHECK(tryInsertKey(table,forward_m(key),forward_m(args)...) == false);
		auto iter = table.find(forward_m(key));
		CHECK( iter != table.end());
		CHECK(iter->second == decltype(iter->second)(forward_m(args)...));
	};

	basic_try_insert_test(table, 0, 1);
	basic_try_insert_test(table, 2, 32);
	CHECK(tryInsertKey(table, 0, 100)==false);
	
	CHECK(setKey(table, 0, 200));
	CHECK(table[0] == 200);

	CHECK(setKey(table, 10, 200) == false);
	CHECK(toKey(table, 10, 200) == 200);
	CHECK(toKey(table, 0, 30) == 200);
	
	
	
	map<std::string, A> table2;
	basic_try_insert_test(table2, "A()", A{});
	CHECK_THROWS_AS(tryInsertKey(table2, "A(1)", 1),int);
	basic_try_insert_test(table2, "A(1)", char(1));
	

}

TEST_CASE("Talg::inner_product") {




}