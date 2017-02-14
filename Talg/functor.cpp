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

#include "test_suits.h"
#include "slotlist.h"



template<class F>
struct Slot:std::function<F>{
	using Base = std::function<F>;
	enum State {
		blocked,enable,disable
	};
	using Base::Base;
	void disconnect() { state_m = State::disable; }
	void block() { state_m = State::blocked; }
	void unblock() { state_m = State::enable; }
	bool is_active()const noexcept { return state_m == State::enable; }
	bool is_destroyed()const noexcept { return state_m == State::disable; }
	State getState()const noexcept { return state_m; }
	template<class T,
		class=std::enable_if_t<std::is_empty<T>::value>>
	bool operator==(const T& rhs){
		auto stored_ptr = Base::template target<T>();
		return stored_ptr!=nullptr;
	}

	/*template<class ObjT>
	bool operator==(ObjT* ptr)const noexcept {
		auto stored_ptr = Base::template target<ObjT>();
		if (stored_ptr) {
			return stored_ptr == ptr;
		}
		return false;
	}*/

	/*template<class ObjT>
	bool operator==(const Slot<ObjT>&)const noexcept{
		return false;
	}
	bool operator==(const Base& )const noexcept {
		return true;
	}*/
private:
	State  state_m = enable;
};

template<class T>
struct DefaultKey:std::tuple<T> {
	using Base = std::tuple<T>;
	static constexpr int default_value = 0;
	using Base::Base;
	operator T&()noexcept { return std::get<0>(*this); }
	operator T&()const noexcept { return std::get<0>(*this); }
};

template<class F, class Key=DefaultKey<int>>
class GrouppedSiganl {
public:
	using SlotType = Slot<F>;
	using Connection = std::shared_ptr<SlotType>;
private:
	std::multimap<Key, Connection> slot_list;
public:
	template<class T>
	Connection connect(T&& f,Key group=Key{Key::default_value}) {
		auto res = std::make_shared<SlotType>(forward_m(f));
		slot_list.insert(std::make_pair(group,res));
		return res;
	}
	template<class...Ts>
	void operator()(Ts&&...args) {
		for(auto iter=slot_list.begin();iter!=slot_list.end();++iter){
			auto& elem=iter->second;
			if(elem->is_active()){
				elem->operator()(forward_m(args)...);
			}else if(elem->is_destroyed()){
				slot_list.erase(iter);
			}
		}
	}
	template<class T>
	void disconnect(const T& f) {
		auto iter=find_if(slot_list.begin(), slot_list.end(), 
			[&f](const std::pair<const Key, Connection>&pair) {
				return *(pair.second) == f;
			}
		);
		if (iter != slot_list.end())
			slot_list.erase(iter);
	}
	void disconnect(const Key& group) {
		slot_list.erase(group);
	}
	void disconnect_all_slots(){
		slot_list.clear();
	}
};


//template<class...Ts>
//using Signal = SignalWrapper<BasicSignal, Ts...>; //SignalWrapper<GrouppedSiganl, Ts...>;

template<class T>
struct ScopedConnection {
	typename GrouppedSiganl<T>::Connection connection;
	~ScopedConnection() {
		connection->disconnect();
	}
};



#if 0
//暂时不知道该如何实现这个特化
template<class ObjT,class MemF,class StandarType>
struct FunctorImp<MemF ObjT::*,StandarType>{
	MemF ObjT::*ptr_m;

	using T = RemoveCvrp<MemF (ObjT::*)>;
	using ParameterIndex =
	std::conditional_t<std::is_same<T, StandarType>::value,
		Seq<>,
		Multiop_n<FindParam,
		typename CallableTraits<T>::arg_type,
		typename CallableTraits<StandarType>::arg_type
		>
	>;
public:
	constexpr FunctorImp( MemF ObjT::*ptr )noexcept
		:ptr_m(ptr) {	}

	template<class...Ts>
	decltype(auto) operator()(Ts&&...args)
		except_when(
			apply(ParameterIndex{}, ptr_m,forward_m(args)...)
		)
	{
		AssertInform(
			"Can't find any parameters match the function call: ",
			NotSame<Seq<NoType>, ParameterIndex>
		);
		return apply(ParameterIndex{}, ptr_m, forward_m(args)...);
	}


	template<class...Ts>
	constexpr decltype(auto) operator()(Ts&&...args)const
		except_when(
			apply(ParameterIndex{}, ptr_m, forward_m(args)...)
		)
	{
		AssertInform(
			"Can't find any parameters match the function call: ",
			NotSame<Seq<NoType>, ParameterIndex>
		);
		return apply(ParameterIndex{}, ptr_m, forward_m(args)...);
	}


	constexpr bool operator==(const FunctorImp& rhs)const noexcept
	{
		return ptr_m == rhs.ptr_m;
	}

	template<class... Us>
	constexpr bool operator==(const FunctorImp<Us...>&)const noexcept {
		return false;
	}

	constexpr bool operator!=(const FunctorImp& rhs)const
		except_when(operator==(rhs))
	{
		return !operator==(rhs);
	}

};
#endif 







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

int main() {
	Signal<void(std::string)> myslot;
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
	//assert(make_f(test::A{})
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
	assert(make_f(callback1) == make_f(callback1));
	assert(make_f(callback1) != make_f(test_fptr));
	assert(make_f(test_fptr2) != make_f(&test_fptr));
	assert(make_f(test::A{}) == make_f(test::A{})); //由于A没有const相等比较,所以采用默认的比较
	assert(make_f(test::B{}) == make_f(test::A{})); 
	assert(make_f(test::A{}) != make_f(test::B{}));
	//assert(make_f(test_fptr) == makeFunctor<void(int)>(test_fptr));

	auto id=myslot.connect(callback1);
	myslot("abcdefg");
	auto id2=myslot.connect( 
		[](std::string str) {
			std::cout <<"2:"<< str << std::endl;
		});
	myslot += [](auto&&...) {};
	//myslot += [](int, std::string) {};
	//StaticAssert<hasOpCall<decltype(blabalabla)>> bbxb;
	//static_assert(!isNonOverloadFunctor<decltype(blabalabla)>::value, "");
	myslot+=test_fptr;
	myslot+=&test_fptr;
	myslot.disconnect(callback1);
	myslot("blabla");

	{
		using namespace test;
		Signal<void(double, char, const std::string&)> sig;
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
			//sig += a;
		}
		//MemFn<A,void(A::*)()>
		//const auto str = std::string("af");
		//std::string&& str_rRef = str;
	}
	return 0;
}



#if 0
void test_fun() {
	boost::signals2::signal<void(double, int)> sig;
	sig.connect([](double, int) {
		return 0;
	});

	sig(1.0, 0);
}

#include <chrono>


namespace bs2 = boost::signals2;
template<class...Ts>
using bs_signal = typename bs2::signal_type<Ts..., bs2::keywords::mutex_type<bs2::dummy_mutex>>::type;

#include <iostream>
	int main22() {
		{
			int i = 0;
			auto ci = [&i](int j){ std::cout << i<<" " << j<<"\n"; return i++; };
			[](auto...) {}(ci(1), ci(2), ci(3));
		}


		auto Timer=[](auto f) {
			using namespace std;
			auto beg = std::chrono::steady_clock::now();
			f();
			decltype(1ms) end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-beg);
			std::cout << end.count() << "ns" << "\n";
		};

		Signals<void(std::string)> myslot;
		auto callback1= [](std::string str) {
			std::cout << str << std::endl;
		};
		auto id=myslot.connect(callback1);
		myslot("abcdefg");
		auto id2=myslot.connect( 
			[](std::string str) {
				std::cout <<"2:"<< str << std::endl;
			});
		myslot.disconnect(callback1);
		myslot("blabla");



		using namespace boost::signals2;
		
		Signal<GrouppedSiganl, void(double,int)> sig0;
		auto play=[&sig0]() {
			std::cout << "!!!+=[]{}";
			sig0 += [] {
				std::cout << "\nanother!\n";
			};
		};
		sig0 += play;
		sig0 -= []() {
			std::cout << "!!!+=[]{}";
			return 0;
		}; 
		sig0(1.3333, 2);
		sig0 -= play;
		sig0(1.3333, 2);


		

		
		//Functor<void()> f{ };
		//boost::signals2::signal<std::function<void()>> sig2;

		
		bs_signal<void(double, int)> sig;
		testSame(Tagi<FindParam<int&&, Seq<double, int>>::value>, Tagi<1>);

		
		auto play2= [&sig](double, int) {
			std::cout << "!!!+=[]{}";
			return sig.connect([](double, int) {
				std::cout << "\nanother!\n";
			});
		};

		Timer([&] {
			for (int i = 0; i < 100; ++i) {
				sig.connect(play2);
			}
			sig.disconnect_all_slots();
		});
		system("pause");
		Timer([&] {
			for (int i = 0; i < 1000000; ++i) {
				sig0 += play;
			}
			sig0.disconnect_all_slots();
		});
		
		sig.connect(makeFunctor<void(double, int)>([](double d) {
			std::cout << "hello!"<<d<<" \n";
		}));
		sig.connect(makeFunctor<void(double, int)>([](int i,double d) {
			std::cout << "hello!"<<i<<" "<<d<<" \n";
		}));
		sig.connect(makeFunctor<void(double, int)>([](const int& i) {
			std::cout << "hello!" << i << " \n";
			return 0;
		}));
		sig.connect(makeFunctor<void(double, int)>( []{
			std::cout << "hello!";
		}));
		apply( 
			Multiop_n<FindParam,
				typename CallableTraits<int(int)>::arg_type,
				typename CallableTraits<int(double, int)>::arg_type
			>{}, [](int) {return 0; },
			1.0,2
			);
		sig(1.0, 0);
		system("pause");
		return 0;
	}

#endif

	/*
	signal<void(double, int)> sig;

	sig.connect(makeFunctor<void(double, int)>([](double,int) {
	std::cout<< "hello!";
	}));
	*/
	//sig();
