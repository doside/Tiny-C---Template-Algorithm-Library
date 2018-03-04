//#include <boost\signals2.hpp>
#include <functional>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <string>
#include <list>
#include <memory>
#include <map>
#include <cassert>

#include "test/test_suits.h"
#include "slotlist.h"

#include "basic_macro.h"


//#include <boost\signals2.hpp>
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
	std::cout << "fptr called \n";
}
void test_fptr2() {
	std::cout << "fptr2 called \n";
}

void assure(bool val){
	if(!val){
		throw std::runtime_error("assert failed");
	}
}

int main() {
	{
		SimpleSignal<void(const char* str,double,int)> sig;
		sig += [](std::string,int) {};
	}

	SimpleSignal<void(std::string)> void_res_slot;

	auto make_f = [](auto&& func) {
		return makeFunctor<void(int, std::string)>(forward_m(func));
	};
	TestConstexpr<
		hasEqualCompare<test::A,test::A&&>::value==true,
		hasEqualCompare<test::A,int>::value==false,
		hasEqualCompare<int,test::A&&>::value==false,
		hasEqualCompare<int,int>::value==true,
		hasEqualCompare<test::B,test::A&>::value==true,
		hasEqualCompare<test::A,test::B&>::value==false
	>{};
	//assure(make_f(test::A{})
	auto Timer=[](auto f) {
		using namespace std;
		auto beg = std::chrono::steady_clock::now();
		f();
		decltype(1ms) end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-beg);
		std::cout << end.count() << "ms" << "\n";
	};

	
	auto callback1= [](std::string str) {
		std::cout << str << std::endl;
	};
	
	static_assert(sizeof(make_f(callback1)) == 1, "");
	static_assert(sizeof(make_f(test_fptr)) == 4, "");
	assure(make_f(callback1) == make_f(callback1));
	assure(make_f(callback1) != make_f(test_fptr));
	assure(make_f(test_fptr2) != make_f(&test_fptr));
	assure(make_f(test::A{}) == make_f(test::A{})); //由于A没有const相等比较,所以采用默认的比较
	assure(make_f(test::B{}) == make_f(test::A{})); 
	assure(make_f(test::A{}) != make_f(test::B{}));

	auto ensure_clear = [&] {
		void_res_slot.disconnect_all();
		assure(void_res_slot.empty());
	};
	//assure(make_f(test_fptr) == makeFunctor<void(int)>(test_fptr));
	{
		SimpleSignal<void(int)> sig;
		sig+ [](){};
		sig(1);
	}
	{
		auto id=void_res_slot.connect(callback1);
		assure(!void_res_slot.empty());
		void_res_slot -= callback1;
		assure(void_res_slot.empty());
	}
	{
		void_res_slot+=test_fptr;
		assure(!void_res_slot.empty());
		void_res_slot+=&test_fptr;
		void_res_slot-=test_fptr;
		assure(void_res_slot.empty());
	}
	{
		void_res_slot+=test_fptr;
		assure(!void_res_slot.empty());
		void_res_slot-=&test_fptr;
		assure(void_res_slot.empty());
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
		assure(!void_res_slot.empty());
		void (*ptr)(std::string) = callback1;
		void_res_slot-=*ptr;
		assure(void_res_slot.empty());
	}*/
	{
		auto con=void_res_slot.connect( 
			[](std::string str) {
				std::cout <<"2:"<< str << std::endl;
			}
		);
		assure(!void_res_slot.empty());
		con->disconnect();
		assure(void_res_slot.empty());
	}
	//void_res_slot("abcdefg");
	
	void_res_slot.disconnect(callback1);


	{
		void_res_slot += [](auto&&...) {};
		void_res_slot -= [](auto&&...) {};
		assure(!void_res_slot.empty());
		ensure_clear();
	}
	{
		std::string str;
		void_res_slot += [&str](auto&& s) {str = s; };
		void_res_slot("blabla");
		assure("blabla" == str);
		
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
		assure(count == 0);
		void_res_slot("");
		void_res_slot -= initer;
		void_res_slot("");
		assure(count == 300);
		void_res_slot("");
		assure(count == 500);
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
		assure(count == 100);
		
		up = 200;
		con->block();
		void_res_slot("");
		assure(count == 100);
		con->unblock();
		void_res_slot("");
		assure(count == 200);
	}
	/*{
		
		
		//using Signal = SignalWrapper<boost::signals2::signal, void()>;
		//Signal sig;
		int count = 0;
		auto cnt = [&] {count++; };
		auto initer= [&sig,cnt] {
			for(int i=0;i<100;++i)
				sig += cnt;
		};
		sig += initer;
		sig();
		assure(count == 0);
		sig();
		assure(count == 100);

		sig -= initer;
		sig();
		assure(count == 300);
		sig();
		assure(count == 500);
	}*/
	
	void_res_slot += test_fptr2;
	//void_res_slot += [](int, std::string) {};
	//StaticAssert<hasOpCall<decltype(blabalabla)>> bbxb;
	//static_assert(!isNonOverloadFunctor<decltype(blabalabla)>::value, "");

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
			//下述用法会失败,因为无法推断参数
			//todo:是否利用LongFirstParser来支持此种用法?
			//test::ConstOp a;
			//(void)a;
			//sig += a;
			sig+=[sig]()mutable { sig.disconnect_all(); };
			sig += [sig] {};
		}
		//MemFn<A,void(A::*)()>
		//const auto str = std::string("af");
		//std::string&& str_rRef = str;
	}
	return 0;
}

