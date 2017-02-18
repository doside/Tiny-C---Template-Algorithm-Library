#pragma once
#include "single_list.h"
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
	//,class =std::enable_if_t<!hasEqualCompare<const F&>::value>
	template<class F>
	bool isEqual(const F& rhs,...)const noexcept
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

	template<class F,
			 class = std::enable_if_t<!std::is_base_of<Base,F>::value>>
	bool operator==(const F& rhs)const 
		except_when(std::declval<EqualableFunction>().isEqual(rhs, 0))
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
		except_when(std::declval<EqualableFunction<Func>>()==rhs)
	{
		return !operator==(rhs);
	}
};



template<class Func>
class BasicSignal{};

struct DefaultResCombiner {
	template<class Iter,class...Ts>
	int operator()(Iter beg,Iter end, Iter real_end, Ts&&...args) {
		for (auto iter = beg; iter != end; ++iter) {
			(*iter)(forward_m(args)...);
		}
		return 0;
	}
};

template<class R,class...Ps>
class BasicSignal<R(Ps...)> {
	using Func = R(Ps...);
	using SlotType = EqualableFunction<Func>;
	using container = SingleList<SlotType>;
	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;
	container slot_list;
public:
	struct Connection{
		const_iterator node;
		container* ref=nullptr;
		Connection(const_iterator link,container& src):node(link),ref(&src){}
		Connection(){}
		void disconnect() {
			ref->erase_after(node);
		}
	};
public:
	BasicSignal()
	:slot_list(){}

	template<class... Ts>
	Connection connect(Ts&&... func) {
		Connection res{ slot_list.before_end(),slot_list };
		slot_list.emplace_back(forward_m(func)...);
		return res;
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
		\note	
	*/
	template<class ResCombiner,class...Ts>
	decltype(auto) visit(ResCombiner&& res_collector,Ts&&...args) {
		//必须插入一个伪结点,而不可以直接用slot_list.before_end(),
		//因为在调用过程中它有可能被删除,并且也不符合半开半闭区间的习惯
		//保存当前最后一个元素的迭代器,用于删除被插入的伪结点
		auto prev_last = slot_list.cbefore_end(); 
		slot_list.push_back(nullptr);
		auto end = slot_list.cbefore_end();//伪装的哨兵结点
		
		//todo: 是否应该auto&& ?		
		auto res=forward_m(res_collector)(
					slot_list.cbegin(), end, 
					slot_list.cend(), forward_m(args)...
				 );
		slot_list.erase_after(prev_last);
		return forward_m(res);
	}

	/*
		\brief	直接调用所有事件
		\note	受到perfect forward的影响,限制了某些推导,
				譬如不可传递{1,2}这种initializer-list作为参数,
				也不可传递位段,因为是总是传递引用,
				对于static member data 会导致odr-use
	*/
	template<class...Ts>
	void operator()(Ts&&...args) {
		visit(DefaultResCombiner{}, forward_m(args)...);
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
		//假定func不可能是直接由*iter得到的,因为会在makeFunctor时复制一份.
		//不直接用remove是因为无法直接EqualableFunction之间是无法直接比较的.
		slot_list.remove_if(
			[&func](const auto& target){
				return target==std::forward<F>(func);	//\note 此处使用了lambda forward_m会失效！
			}
		);
	}
	void disconnect_all() {
		slot_list.clear();
	}
	bool empty()const noexcept {
		return slot_list.empty();
	}
};

template<class...Ts>
using SimpleSignal = SignalWrapper<BasicSignal, Ts...>;


template<class...Ts>
using Signal = SignalWrapper<BasicSignal, Ts...>; 

