#pragma once
#include "signal_wrapper.h"
#include "has_member.h"

template<class Func>
struct EqualableFunction :std::function<Func> {
	using Base = std::function<Func>;
	using Base::Base;
private:
	template<class F>
	bool isEqual(const F& rhs,EnableIfT<hasEqualCompare<const F&>>*)const 
		except_when(std::declval<const F&>()==std::declval<const F&>())
	{
		if(auto ptr = Base::template target<F>()) {
			return *ptr==rhs;
		}
		return  false;
	}
	template<class F,
		class =std::enable_if_t<
		!hasEqualCompare<const F&>::value
		>
	>
	bool isEqual(const F& rhs,int)const noexcept
	{
		return std::is_same<Func, F>::value;
	}
public:
	template<class Src,class Other>
	bool compare(Src&&,const Other& rhs)
		except_when(std::declval<const Src&>()==std::declval<const Other&>())
	{
		if(auto ptr = Base::template target<Src>()) {
			return *ptr==rhs;
		}
		return  false;
	}

	template<class F>
	bool operator==(const F& rhs)const 
		except_when(std::declval<EqualableFunction>().isEqual(rhs, 0))
	{
		return isEqual(rhs, 0);
	}


	template<class F>
	constexpr bool operator==(const std::function<F>& rhs)const noexcept
	{
		static_assert(std::is_same<const std::function<F>&,decltype(rhs)>::value,
					"Can't support such comparation");
		return false;
	}

	template<class F>
	constexpr bool operator!=(const F& rhs)const
		except_when(std::declval<EqualableFunction<Func>>()==rhs)
	{
		return !operator==(rhs);
	}
};


/*
template<class StandarType,class T,class R,class...Ps>
constexpr decltype(auto) makeFunctor(std::weak_ptr<T> ptr,R (T::*pmd)(Ps...)) {
	return	FunctorImp<CrtpMaker<decltype(ptr),decltype(pmd)>, StandarType>(
				CrtpMaker<std::weak_ptr<T>,decltype(pmd)>(std::move(ptr),pmd)
			);
}*/


template<class Func>
class BasicSignal {
	using SlotType = EqualableFunction<Func>;
	using container = std::forward_list<SlotType>;
	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;
	container slot_list;
	iterator last;
public:
	struct Connection{
		iterator node;
		container* ref=nullptr;
		Connection(iterator link,container& src):node(link),ref(&src){}
		Connection(){}
		void disconnect() {
			ref->erase_after(node);
		}
	};
public:
	BasicSignal()
	:slot_list(),last(slot_list.before_begin()){}

	template<class... Ts>
	Connection connect(Ts&&... func) {
		slot_list.insert_after(last,forward_m(func)...);
		return Connection(last++, slot_list);
	}
	template<class...Ts>
	void operator()(Ts&&...args) {
		for(auto&elem:slot_list){
			elem(forward_m(args)...);
		}
	}
	template<class F>
	void disconnect(F&& func) {
		auto iter = slot_list.cbegin();
		auto prev = slot_list.cbefore_begin();
		for(;iter!=slot_list.cend();++iter){
			if (*iter==forward_m(func)) {
				slot_list.erase_after(prev);
				break;
			}
			prev = iter;
		}
	}
	template<class F>
	void disconnect_all(F&& func) {
		slot_list.remove(forward_m(func));
	}
	void disconnect_all() {
		slot_list.clear();
	}
};

template<class...Ts>
using SimpleSignal = SignalWrapper<BasicSignal, Ts...>;


template<class...Ts>
using Signal = SignalWrapper<BasicSignal, Ts...>; 

