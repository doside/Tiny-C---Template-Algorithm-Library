#include <functional>
#include "core.h"
#include "test_suits.h"
#include "strippe_qualifier.h"
#include "find_val.h"
#include "callable_traits.h"

#include <iostream>
#include <boost\signals2.hpp>
#include <map>


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

	template<class P>
	bool operator==(P* ptr)const noexcept {
		auto stored_ptr = Base::template target<P>();
		if (stored_ptr) {
			return stored_ptr == ptr;
		}
		return false;
	}

	template<class P>
	bool operator==(const Slot<P>&)const noexcept{
		return false;
	}
	bool operator==(const Base& )const noexcept {
		return true;
	}
private:
	State  state_m = enable;
};

template<class T>
struct DefaultKey:std::tuple<T> {
	using Base = std::tuple<T>;
	static constexpr int default_value = 0;
	using Base::Base;
	operator T&()noexcept { return get<0>(*this); }
	operator T&()const noexcept { return get<0>(*this); }
};

template<class F, class Key=DefaultKey<int>>
class Signals {
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
		auto iter=find_if(slot_list.begin(), slot_list.end(), [&f](const auto&pair) {
			return *(pair.second) == f;
		});
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

template<class T>
struct ScopedConnection {
	typename Signals<T>::Connection connection;
	~ScopedConnection() {
		connection->disconnect();
	}
};


struct MatchParam
{
	template<class T, class U>
	struct pred
	{
	private:
		static U declParam();
		static int f(T);
		static void f(...);
		//std::is_same<int, decltype(f(declParam()))>::value;
	public:
		static constexpr bool value = std::is_same<std::decay_t<U>, std::decay_t<T>>::value
			&& std::is_convertible<U,T>::value;
	
	};
};

template<class T,class S>
using FindParam = Find_if_svt<MatchParam, T, S>;

//template<class T, class S>using FindParam_s = OMIT_T( Find_if_svt<MatchParam, T, S>);





template<class F>
struct CallableTraits :
	std::conditional_t<std::is_function<F>::value,
	CallableTraits<RemoveCvrp<F>>,
	CallableTraits<RemoveCvrp<decltype(& std::remove_reference_t<F>::operator())>>
	> 
{ };

template<class R ,class...Ts>
struct CallableTraits<R(Ts...)>:std::integral_constant<size_t,sizeof...(Ts)> {
	using arg_type = Seq<Ts...>;
	using ret_type = R;
};

template<class R, class...Ts>
struct CallableTraits<std::function<R(Ts...)>> : CallableTraits<R(Ts...)>{};


template<class T, class StandarType>
struct FunctorImp
{
	static_assert(!std::is_reference<T>::value, "we assume T is not a ref.");
	using ParameterIndex = 
		std::conditional_t<std::is_same<T,StandarType>::value,
		Seq<>,
		Multiop_n<FindParam,
			typename CallableTraits<T>::arg_type,
			typename CallableTraits<StandarType>::arg_type
		>
	>;
	T func_m;
public:
	constexpr FunctorImp(const T& f)noexcept(std::is_nothrow_copy_constructible<T>::value)
		:func_m(f)
	{

	}
	constexpr FunctorImp(T&& f)noexcept(std::is_nothrow_move_constructible<T>::value)
		: func_m(std::move(f))
	{

	}

	template<class...Ts>
	decltype(auto) operator()(Ts&&...args)
		except_when(
			apply(ParameterIndex{},func_m, forward_m(args)...)
		)
	{
		return apply(ParameterIndex{},func_m, forward_m(args)...);
	}


	template<class...Ts>
	constexpr decltype(auto) operator()(Ts&&...args)const
		except_when(
			apply(ParameterIndex{}, func_m, forward_m(args)...)
		)
	{
		return apply(ParameterIndex{}, func_m, forward_m(args)...);
	}
#if 0
	constexpr auto operator==(const FunctorImp& rhs)const
		except_when(func_m == rhs.func_m)
		->decltype(func_m == rhs.func_m)
	{
		return func_m == rhs.func_m;
	}
#endif

	template<class=void>
	constexpr bool operator==(const FunctorImp& rhs)const noexcept
	{
		return true;
	}

	constexpr bool operator!=(const FunctorImp& rhs)const
		except_when(operator==(rhs))
	{
		return !operator==(rhs);
	}

};

template<class P,class Obj,class StandarType>
struct FunctorImp<Obj P::*,StandarType>:private P{
	Obj P::*ptr_m;

	using T = RemoveCvrp<Obj (P::*)>;
	using ParameterIndex =
	std::conditional_t<std::is_same<T, StandarType>::value,
		Seq<>,
		Multiop_n<FindParam,
		typename CallableTraits<T>::arg_type,
		typename CallableTraits<StandarType>::arg_type
		>
	>;
public:
	constexpr FunctorImp(P&& val, Obj(P::*ptr))noexcept(std::is_nothrow_move_constructible<P>::value)
	:P(std::move(val)),ptr_m(ptr){

	}
	constexpr FunctorImp(const P& val, Obj(P::*ptr))noexcept(std::is_nothrow_copy_constructible<P>::value)
		:P(val), ptr_m(ptr) {

	}
	template<class...Ts>
	decltype(auto) operator()(Ts&&...args)
		except_when(
			apply(ParameterIndex{}, ptr_m, std::declval<FunctorImp&>(), forward_m(args)...)
		)
	{
		AssertInform(
			"Can't find any parameters match the function call: ",
			NotSame<Seq<NoType>, ParameterIndex>
		);
		return apply(ParameterIndex{}, ptr_m, *this, forward_m(args)...);
	}


	template<class...Ts>
	constexpr decltype(auto) operator()(Ts&&...args)const
		except_when(
			apply(ParameterIndex{}, ptr_m, std::declval<const FunctorImp&>(), forward_m(args)...)
		)
	{
		AssertInform(
			"Can't find any parameters match the function call: ",
			NotSame<Seq<NoType>, ParameterIndex>
		);
		return apply(ParameterIndex{}, ptr_m, *this, forward_m(args)...);
	}

#if 0
	constexpr auto operator==(const FunctorImp& rhs)const
		except_when( P::operator==(rhs) )
		->decltype( P::operator==(rhs) )
	{
		return ptr_m == rhs.ptr_m && P::operator==(rhs);
	}
#endif

	template<class=void>
	constexpr bool operator==(const FunctorImp& rhs)const noexcept
	{
		return true;
	}

	constexpr bool operator!=(const FunctorImp& rhs)const
		except_when(operator==(rhs))
	{
		return !operator==(rhs);
	}

};


template<class F, class T>
constexpr decltype(auto) makeFunctor(T&& src_func) {
	return FunctorImp<std::remove_reference_t<T>, F>(forward_m(src_func));
}

template<class F, class T>
decltype(auto) makeFunctor2(T&& src_func)
	except_when( std::remove_reference_t<T>(forward_m(src_func)) )
{
	using ParameterIndex = Multiop_n<FindParam,
		typename CallableTraits<T>::arg_type,
		typename CallableTraits<F>::arg_type
	>;

	AssertInform(
		"Can't find any parameters match the function call: ",
		NotSame<Seq<NoType>,ParameterIndex>
	);

	//!! we don't use type alias in lambda, because it is possible to cause some compiler bug.
	return [src_func=forward_m(src_func)](auto&&...args) {
		apply(Multiop_n<FindParam,
			typename CallableTraits<T>::arg_type,
			typename CallableTraits<F>::arg_type
		>{},src_func, forward_m(args)...);
	};
}

template<class F>
struct Functor:public std::function<F> 
{
	using Base = std::function<F>;

	//using Base::Base;
	//using Base::swap;
	//using Base::operator bool;
	//using Base::operator=;

	Functor():Base(){}
	template<class T>
	Functor(T&& f)
	:Base(
		makeFunctor<T>(forward_m(f))
	) { }
};


template<template<class...>class Sig, class...Ts> 
class Signal:public Sig<Ts...>
{
	using Base = Sig<Ts...>;
public:
	using ftype = Head_s<Seq<Ts...>>;
	using Base::Base;
	template<class F>
	decltype(auto) operator+=(F&& func) 
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::connect(makeFunctor<ftype>(forward_m(func)));
	}
	template<class F>
	decltype(auto) operator-=(F&& func)
		except_when(std::declval<Base&>().connect(makeFunctor<ftype>(forward_m(func))))
	{
		return Base::disconnect(makeFunctor<ftype>(forward_m(func)));
	}

};


















#if 0
void test_fun() {
	boost::signals2::signal<void(double, int)> sig;
	sig.connect([](double, int) {
		return 0;
	});

	sig(1.0, 0);
}
#else
#include <chrono>



namespace bs2 = boost::signals2;
template<class...Ts>
using bs_signal = typename bs2::signal_type<Ts..., bs2::keywords::mutex_type<bs2::dummy_mutex>>::type;

#include <iostream>
	int main() {
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
		
		Signal<Signals, void(double,int)> sig0;
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
		testSame(iTag<FindParam<int&&, Seq<double, int>>::value>, iTag<1>);

		
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
