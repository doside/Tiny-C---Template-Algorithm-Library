#include <doctest/doctest.h>
#include <Talg/slotlist.h>
#include <Talg/basic_macro.h>
using namespace Talg;
	namespace test {
		struct A
		{
			void operator()(){}
			bool operator==(const A&) { return false; }
		};
		struct B
		{
			void operator()(){}
			bool operator==(const A&)const { return true; }
		};

		struct ConstOp {
			void operator()(){}
			void operator()()const { }
		};

	}


void test_fptr() {
	//std::cout << "fptr called \n";
}
void test_fptr2() {
	//std::cout << "fptr2 called \n";
}
#define require REQUIRE
TEST_CASE("Connection Test") {
	SimpleSignal<void(std::string)> void_res_slot;
	auto ensure_clear = [&] {
		void_res_slot.disconnect_all();
		require(void_res_slot.empty());
	};	
	
	auto make_f = [](auto&& func) {
		return makeFunctor<void(int, std::string)>(forward_m(func));
	};

	auto callback1= [](std::string str) {
		//std::cout << str << std::endl;
	};
	
	static_assert(sizeof(make_f(callback1)) == 1, "");
	static_assert(sizeof(make_f(test_fptr)) == 4, "");
	require(make_f(callback1) == make_f(callback1));
	require(make_f(callback1) != make_f(test_fptr));
	if (test_fptr2 != &test_fptr) {
		require(make_f(test_fptr2) != make_f(&test_fptr));	//release版本直接通不过.
	}
	require(make_f(test::A{}) == make_f(test::A{})); //由于A没有const相等比较,所以采用默认的比较
	require(make_f(test::B{}) == make_f(test::A{})); 
	require(make_f(test::A{}) != make_f(test::B{}));


	//require(make_f(test_fptr) == makeFunctor<void(int)>(test_fptr));
	{
		SimpleSignal<void(int)> sig;
		sig+ [](){};
		sig(1);
	}
	{
		auto id=void_res_slot.connect(callback1);
		require(!void_res_slot.empty());
		void_res_slot -= callback1;
		require(void_res_slot.empty());
	}
	{
		void_res_slot+=test_fptr;
		require(!void_res_slot.empty());
		void_res_slot+=&test_fptr;
		void_res_slot-=test_fptr;
		require(void_res_slot.empty());
	}
	{
		void_res_slot+=test_fptr;
		require(!void_res_slot.empty());
		void_res_slot-=&test_fptr;
		require(void_res_slot.empty());
	}
	{
		auto con1 = void_res_slot + test_fptr;
		auto con2 = void_res_slot + &test_fptr;
		con1->disconnect();
		con2->disconnect();
		ensure_clear();
	}
	/*{
		void_res_slot+=callback1;
		require(!void_res_slot.empty());
		void (*ptr)(std::string) = callback1;
		void_res_slot-=*ptr;
		require(void_res_slot.empty());
	}*/
	{
		auto con=void_res_slot.connect( 
			[](std::string str) {
				//std::cout <<"2:"<< str << std::endl;
			}
		);
		require(!void_res_slot.empty());
		con->disconnect();
		require(void_res_slot.empty());
	}
	//void_res_slot("abcdefg");
	
	void_res_slot.disconnect(callback1);


	{
		void_res_slot += [](auto&&...) {};
		void_res_slot -= [](auto&&...) {};
		require(!void_res_slot.empty());
		ensure_clear();
	}
	{
		std::string str;
		void_res_slot += [&str](auto&& s) {str = s; };
		void_res_slot("blabla");
		require("blabla" == str);
		
	}
	{
		int count = 0;
		auto cnt = [&] {count++; };
		auto initer= [&void_res_slot,cnt] {
			for(int i=0;i<100;++i)
				void_res_slot += cnt;
		};
		void_res_slot += initer;
		void_res_slot("");
		require(count == 0);
		void_res_slot("");
		void_res_slot -= initer;
		void_res_slot("");
		require(count == 300);
		void_res_slot("");
		require(count == 500);
		ensure_clear();
	}

	{
		ensure_clear();
		int count = 0;
		int up = 100;
		auto con=void_res_slot + [&] {
			for(;count<up;++count) { }
		};
		void_res_slot("");
		require(count == 100);
		
		up = 200;
		con->block();
		void_res_slot("");
		require(count == 100);
		con->unblock();
		void_res_slot("");
		require(count == 200);
	}

	
	void_res_slot += test_fptr2;

	{
		using namespace test;
		SimpleSignal<void(double, char, const std::string&)> sig;
		sig += [](const std::string&, char) {return 1; };
		struct A { 
			void f(double,char){}
		};
		A a;
		sig.connect(&a, &A::f);
		{
			test::ConstOp a;
			sig+=[sig]()mutable { sig.disconnect_all(); };
			sig += [sig] {};
		}

	}

}
TEST_CASE("Functor Wrapper") {
	SimpleSignal<void(double, int)> sig;
	sig += [](int lhs, double rhs) {
		CHECK(lhs == 1);
		CHECK(rhs == 2.5);
	};
	sig(2.5, 1);
	sig += [](int lhs, auto rhs) {
		CHECK(lhs == 2);
		CHECK(rhs == 1);
	};
	sig(2.5, 1);
}