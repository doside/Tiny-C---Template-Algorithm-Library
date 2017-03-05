#pragma once
#include "iter_call_cache.h"
#include <functional>
#include <cassert>

namespace Talg{
template<class GetParram,class Cache,class Iterator>
class SlotCallIterator{
public:
	using reference_type	= typename Cache::reference_type;
	using value_type		= typename Cache::value_type;
	using pointer			= typename Cache::pointer;
	//采用inputiterator,因为允许op++使迭代器失效.并且不是default constructable
	using iterator_category = std::input_iterator_tag;	
	using difference_type	= typename Iterator::difference_type;
protected:
	Iterator prev_;
	std::reference_wrapper<GetParram> call;
	std::reference_wrapper<Cache> cache;
public:
	SlotCallIterator(const Iterator& iter,GetParram& f1,Cache& f2)
	:prev_(iter),call(f1),cache(f2){

	}

	reference_type operator*(){
		return call(cache,std::next(prev_));
	}
	explicit operator bool()const noexcept {
		return std::next(prev_)->is_callable();
	}

	friend bool operator==(const SlotCallIterator& lhs, const SlotCallIterator& rhs)
	noexcept(noexcept(lhs.prev_==rhs.prev_))
	{
		return lhs.prev_ == rhs.prev_;
	}
	friend bool operator!=(const SlotCallIterator& lhs, const SlotCallIterator& rhs)
	noexcept(noexcept(lhs==rhs))
	{
		return !(lhs==rhs);
	}

	pointer operator->()const 
	noexcept(noexcept(std::declval<SlotCallIterator&>().operator*()))
	{
		return std::addressof(**this);
	}

	SlotCallIterator& operator++()
	noexcept(noexcept(std::declval<Cache>().reset())) 
	{
		++prev_;
		cache.get().reset();
		return *this;
	}
	
	
	SlotCallIterator operator++(int)
	noexcept(noexcept(
		++ std::declval<SlotCallIterator<GetParram,Cache,Iterator>&>()
	))
	{
		static_assert(std::is_nothrow_copy_constructible<SlotCallIterator>::value,
			"We assume iterator is_nothrow_copy_constructible for noexcept");
		SlotCallIterator old = *this;
		++(*this);
		return old;
	}

	
};



template<class R,class Iter,class T,class T2>
auto makeSlotIter(const Iter& iter, T& get_param,T2& cache) {
	return SlotCallIterator<T,T2,Iter>{iter, get_param,cache};
}



template<class GetParram,class Cache,class Iterator>
class CheckCallIterator:public SlotCallIterator<GetParram,Cache,Iterator>{
	using Base = SlotCallIterator<GetParram, Cache, Iterator>;
	using reference_type=typename Base::reference_type;
	using container = std::remove_pointer_t<decltype(std::declval<Iterator>()->state->ref)>;
	container& con;
	bool has_locked=false;
public:
	using Base::Base;
	CheckCallIterator(const Base& rhs,container& c)noexcept(std::is_nothrow_copy_constructible<Base>::value)
		:Base(rhs),con(c){}
	reference_type operator*(){
		if (has_locked==false) {
			auto iter = std::next(Base::prev_);
			using State=std::remove_pointer_t<decltype(iter->state)>;
			State state(Base::prev_, con);
			auto guard=iter->lock(state);
			has_locked = true;
			return call(Base::cache,iter);
		}
		return Base::operator*();
	}
	CheckCallIterator& operator++()
	{
		Base::operator++();
		has_locked = false;
		return *this;
	}
	
	CheckCallIterator operator++(int)
	{
		CheckCallIterator old = *this;
		++(*this);
		return old;
	}
};

}//namespace Talg

