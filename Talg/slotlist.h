#pragma once
#include "signal_wrapper.h"

template<class Func>
struct EqualableFunction :std::function<Func> {
	using Base = std::function<Func>;
	using Base::Base;
	template<class F>
	bool operator==(const F& rhs)const 
		except_when(std::declval<const F&>()==std::declval<const F&>())
	{
		if (!hasEqualCompare<const F&>::value)
			return false;
		auto ptr = Base::template target<F>();
		if (ptr == nullptr) {
			return false;
		}
		return  *ptr==rhs;
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

	template<class T>
	Connection connect(T&& func) {
		slot_list.insert_after(last,forward_m(func));
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

};

template<class...Ts>
using SimpleSignal = SignalWrapper<BasicSignal, Ts...>;

template<class Obj,class DataT>
struct MemFn
{

};
