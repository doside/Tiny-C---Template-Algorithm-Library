#pragma once
#include "single_list.h"
#include "signal_wrapper.h"
#include "has_member.h"
#include "slot_iterator.h"
#include <type_traits>
#include "basic_marco_impl.h"

namespace Talg {



template<class Signature_>
struct EqualableFunction :std::function<Signature_> {
	using Base = std::function<Signature_>;
	using Base::Base;
	using Signature = Signature_;
private:
	template<class F>
	bool isEqual(const F& rhs,EnableIfT<hasEqualCompare<const F&>>*)const 
		except_when_m(std::declval<const F&>()==std::declval<const F&>())
	{
		if(auto ptr = Base::template target<F>()) {
			return *ptr==rhs;
		}
		return  false;
	}
	//,class =std::enable_if_t<!hasEqualCompare<const F&>::value>
	template<class F>
	bool isEqual(const F& rhs,...)const noexcept
	{
		return std::is_same<Signature, F>::value;
	}
public:
	template<class Src,class Other>
	bool compare(Src&&,const Other& rhs)
		except_when_m(std::declval<const Src&>()==std::declval<const Other&>())
	{
		if(auto ptr = Base::template target<Src>()) {
			return *ptr==rhs;
		}
		return  false;
	}

	template<class F,
			 class = std::enable_if_t<!std::is_base_of<Base,F>::value>>
	bool operator==(const F& rhs)const 
		except_when_m(std::declval<EqualableFunction>().isEqual(rhs, 0))
	{
		return isEqual(rhs, 0);
	}
	template<class F>
	constexpr bool operator==(const std::function<F>& rhs)const noexcept
	{
		//随便写条表达式让断言失败,不写false是为了防止过早实例化时断言失败
		static_assert( sizeof(rhs)==0,"Can't support such comparation");
		return false;
	}

	template<class F>
	constexpr bool operator!=(const F& rhs)const
		except_when_m(std::declval<EqualableFunction<Signature_>>()==rhs)
	{
		return !operator==(rhs);
	}
};



template<class Functor>
struct DefaultSlotTraits {

	/*
		\brief	为了支持BasicSignal<Ret(Ps),Traits<>>,
				其中Traits<T>为某个DefaultSlotTraits<function<T>>的别名,
				从而避免类似这样的冗余写法:BasicSignal<Ret(Ps),Traits<Ret(Ps)> >
				同时也是为了方便更换function而不需要改变slottraits的实现.
		\param	T真正的类型,一般情况下Functor就是std::function<T>,rebind之后也一样,
				对于某些特殊情况原本Functor是std::function<void>,rebind之后变成function<T>
		\require	要么是DefaultSlotTraits本身要么是DefaultSlotTraits<ReplaceParam<Functor, T>>;
	*/
	template<class T>
	using rebind = DefaultSlotTraits<ReplaceParam<Functor, T>>;

	enum SlotState:char{
		free=0,discon=2,blocked=4,locked=8
	};
	struct State;
	//using Functor = ReplaceParam<Functor_, Signature>;
	struct SlotType : Functor {
		using Base = Functor;
		State* state;
		template<class...Ts>
		SlotType(State* s, Ts&&...args)
			: Base(forward_m(args)...),state(s)
		{

		}
		SlotType(SlotType&&) = default;
		SlotType(const SlotType&) = default; //todo: forbid such function.
		SlotType& operator=(const SlotType&) = default;
		SlotType& operator=(SlotType&&) = default;
		auto lock(State& other);
		bool is_callable()const noexcept;
		~SlotType();
	};
	using container = SingleList<SlotType>;
	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;
	
	struct State {
		container* ref=nullptr;
		iterator prev_node_;
		//iterator next_node_;
		SlotState state=free;
		
		
		State(iterator link,container& src)
			:ref(&src),prev_node_(link)//,next_node_(prev_node_)
		{		}

		//State(const State&)noexcept = delete;
		State(State&& rhs)noexcept
		:ref(rhs.ref),prev_node_(rhs.prev_node_),state(rhs.state)
		{		}

		//State& operator=(const State&)noexcept = delete;
		State& operator=(State&& rhs)noexcept {	
			//MSVC_BUG: 使用=default的话不符合noexcept
			//一般情况下还无法复现这个问题.
			ref = rhs.ref;
			prev_node_ = rhs.prev_node_;
			state = rhs.state;
			return *this;
		}
		void lock(){
			//precondition: signal is emitting.
			state = locked;
			//seek_next();
		}
		void block(){
			if (state != discon) {
				state = blocked;
			}
		}
		void unblock()noexcept {
			if (state != discon) {
				state = free;
			}
		}
		bool is_blocked()const noexcept{
			return state == blocked || state==locked;
		}
		bool is_disconnected()const noexcept {
			return state == discon;
		}
		bool is_connected()const noexcept {
			return state != discon;
		}

		SlotState disconnect() {
			if (state == blocked || state == locked || state == discon) {
				return state;
			}
			if (state != discon) {
				ref->erase_after(prev_node_);
				iterator old = prev_node_++;	//此处使用auto时曾经引发未知的MSVC的bug
				if (prev_node_ == ref->end()) {
					state = discon;
					return discon;
				}
				if(prev_node_->state != nullptr) {
					prev_node_->state->prev_node_ = old;
				}
			}
			state = discon;
			return discon;
		}
		~State() {
			static_assert(std::is_nothrow_move_constructible<State>::value &&
							std::is_nothrow_move_assignable<State>::value, "oh no, compiler bug");
			if (state != discon) {
				auto next=std::next(prev_node_);
				next->state = nullptr;
			}
		}
	};
	
	using Connection = std::unique_ptr<State>;
	using SharedConnection = std::shared_ptr<State>;

	struct DefaultResCombiner {
		template<class Iter,class Container>
		decltype(auto) operator()(Iter first,Iter last,const Container&) {
			for (; first != last ; ++first) {
				if (!first){
					continue;
				}
				*first;
			}
		}
	};
};
template<class Functor>
 DefaultSlotTraits<Functor>::SlotType::~SlotType() {
	if (state != nullptr) {
		state->state = discon;
	}
}
template<class Functor>
auto DefaultSlotTraits<Functor>::SlotType::lock(State& other){
	assert(
		state==nullptr || (!(state->is_blocked()) && state->is_connected())
	);
	State* old = nullptr;
	if (state == nullptr) {
		state = &other;
	}else {
		old = state;
	}
	state->lock();
	auto when_exit = [old,this](auto* ) {
		state->unblock();
		state = old;
	};
	return std::unique_ptr<State, decltype(when_exit)> ( state, when_exit );
}

template<class Functor>
bool DefaultSlotTraits<Functor>::SlotType::is_callable()const noexcept {
	return state == nullptr || state->state == free;
}


template<class Signature,
		 class SlotTrait=DefaultSlotTraits<EqualableFunction<Signature>>
>
class BasicSignal;




template<class R,class...Ps,class SlotTrait_>
class BasicSignal<R(Ps...),SlotTrait_>:private SlotTrait_::template rebind<R(Ps...)>{
public:
	using SlotTrait = typename SlotTrait_::template rebind<R(Ps...)>;
	using Base = SlotTrait;
	using SlotType = typename SlotTrait::SlotType;
	using State = typename SlotTrait::State;
	using container = typename SlotTrait::container;
	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;
private:
	container slot_list;
public:
	using Connection = typename SlotTrait::Connection;
	using SharedConnection = typename SlotTrait::SharedConnection;
	using DefaultResCombiner = typename SlotTrait::DefaultResCombiner;
public:
	BasicSignal()
	:slot_list(){

	}

	template<class... Ts>
	Connection connect(Ts&&... func) {
		//Connection res{ slot_list.before_end(),slot_list };
		auto con = std::make_unique<State>(slot_list.before_end(), slot_list);
		slot_list.emplace_back(con.get(),forward_m(func)...);
		return con;
	}



	/*
		\brief	用于让任意类型的结果组合调用所有的当前事件槽,
				并且在该调用过程中,如果有新的事件被添加,不会
				在此次调用中呼叫,而是留待下一次.允许在此过程中
				删除某个事件,但不可 以空参数 调用disconnect_all.
		\param	res_collector结果组合器,类似于boost signals,
				提供3个迭代器参数,其中end是当前的end,real_end是真正的end
				但是对其的间接引用不引发函数调用,需要显式地调用args...
		\return 返回res_collector的调用结果.
		\note	last不是slot,组合器绝不可解引用它,否则行为未定义
	*/
	template<class ResCombiner,class...Ts>
	auto visit(ResCombiner&& res_collector,Ts&&...args) {
		forward_m(res_collector)(
					slot_list.cbegin(),
					slot_list.cend(), 
					slot_list,
					slot_list.cbefore_end(), 
					forward_m(args)...
				 );
	}

	template<class ResCombiner,class Cache,class Iter,class...Ts>
	decltype(auto) call(ResCombiner&& res_collector,Cache& cache,const Iter& before_beg,const Iter& before_end, Ts&&...args) 
	{
		auto getter=[&args...](Cache& cache,const Iter& iter)->typename Cache::reference_type
			{
				if (!cache) {
					cache.reset(iter, std::forward<Ts>(args)...);
				}
				return cache.get();
			};	
		 
		return forward_m(res_collector)(
			makeSlotIter<R>(before_beg, getter, cache),
			makeSlotIter<R>(before_end,getter,cache),
			*this
		);
	}


	template<class ResCombiner,class...Ts>
	decltype(auto) collect(ResCombiner&& res_collector,Ts&&...args) {
		CacheRes<R> cache{};
		return call(forward_m(res_collector), cache,
			slot_list.before_begin(), 
			slot_list.before_end(),
			forward_m(args)...);
	}



	/*
		\brief	直接调用所有事件
		\note	受到perfect forward的影响,限制了某些推导,
				譬如不可传递{1,2}这种initializer-list作为参数,
				也不可传递位段,因为是总是传递引用,
				对于static member data 会导致odr-use
	*/
	template<class...Ts>
	decltype(auto) operator()(Ts&&...args) {
		CacheRes<R> cache{};
		return call(DefaultResCombiner{}, cache,
			slot_list.cbefore_begin(), 
			slot_list.cbefore_end(),
			forward_m(args)...);
	}

	/*
		\brief	在某些情况下无法forward,所以提供该函数来方便其emit
		\param	参数类型Ps...
		\note	如果是引用的话,并不会有复制开销,但如果是值类型则会
				有多余的复制产生,因为std::function也像这样复制了参数.
	*/
	void emit(Ps...args) {
		visit(DefaultResCombiner{}, forward_m(args)...);
	}
	template<class F>
	void disconnect_one(F&& func) {
		auto iter = slot_list.begin();
		auto prev = slot_list.before_begin();
		auto end=slot_list.end();
		while (iter != end) {
			if (*iter==forward_m(func)) {
				iter=slot_list.erase_after(prev);
				if (iter != end && iter->state != nullptr) {
					iter->state->prev_node_ = prev;
				}
				break;
			} else {
				prev = iter;
				++iter;
			}
		}
	}
	template<class F>
	void disconnect(F&& func) {
		//假定func不可能是直接由*iter得到的,因为会在makeFunctor时复制一份.
		//不直接用remove是因为EqualableFunction之间是无法直接比较的.
		auto iter = slot_list.begin();
		auto prev = slot_list.before_begin();
		auto end=slot_list.end();
		while (iter != end) {
			if (*iter==forward_m(func)) {
				iter=slot_list.erase_after(prev);
				if (iter == end)
					break;
				if (iter->state != nullptr) {
					iter->state->prev_node_ = prev;
				}
			} else {
				prev = iter;
				++iter;
			}
		}
	}
	void disconnect_all() {
		//在此过程中有些connection处于非正常状态,如果有线程在此时访问任何connection都会未定义行为
		slot_list.clear();	
	}
	bool empty()const noexcept {
		return slot_list.empty();
	}

	template<class Sig,class GetParram,class Cache,class Iterator>
	friend CheckCallIterator<GetParram, Cache, Iterator> 
	makeLockedIter(const SlotCallIterator<GetParram, Cache, Iterator>& iter, Sig& c);
};

#if 0
//无法提取出signature
template<class SlotTrait>
class BasicSignal<void,SlotTrait>
	:protected BasicSignal<
		Signature,
		typename SlotTrait::template rebind<Signature>
	>
{

public://type
	using Base = BasicSignal<
		Signature,
		typename SlotTrait::template rebind<Signature>
	>;
	using Base::Base;
	using Base::SlotType;
	using Base::State;
	using Base::container;
	using Base::iterator;
	using Base::const_iterator;
	using Base::Connection;
	using Base::SharedConnection;
	using Base::DefaultResCombiner;
public://method
	using Base::connect;
	using Base::disconnect;
	using Base::disconnect_all;
	using Base::disconnect_one;
	using Base::emit;
	using Base::operator();
	using Base::visit;
	using Base::collect;
	using Base::empty;
	using Base::call;
};
#endif


template<class...Ts>
using SimpleSignal = SignalWrapper<BasicSignal, Ts...>;

template<class Sig,class GetParram,class Cache,class Iterator>
CheckCallIterator<GetParram, Cache, Iterator>  
makeLockedIter(const SlotCallIterator<GetParram,Cache,Iterator>& iter,Sig& c) {
	return CheckCallIterator<GetParram, Cache, Iterator>(iter,c.slot_list);
}

//template<class...Ts>
//using Signal = SignalWrapper<BasicSignal, Ts...>; 

}//namespace Talg

#include "undef_macro.h"
