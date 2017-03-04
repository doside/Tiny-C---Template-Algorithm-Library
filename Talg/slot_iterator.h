﻿#pragma once
#include "iter_call_cache.h"
#include <functional>
#include <cassert>

template<class Iter>
bool is_callable_iterator(const Iter& iter) {
	//precondition iter in range [begin,end).
	return iter->state == nullptr || !(iter->state->is_blocked());
}

template<class R,class Func,class Func2,class Iterator>
class SlotCallIterator{
	using reference_type	= typename CacheRes<R>::reference_type;
	using value_type		= typename CacheRes<R>::value_type;
	using pointer			= typename CacheRes<R>::pointer;
	//采用inputiterator,因为允许op++使迭代器失效.并且不是default constructable
	using iterator_category = std::input_iterator_tag;	
	using difference_type	= typename Iterator::difference_type;
	Iterator prev_;
	std::reference_wrapper<Func> cache_;
	std::reference_wrapper<Func2> seeker_;
public:
	SlotCallIterator(const Iterator& iter,Func& f1,Func2& f2)
	:prev_(iter),cache_(f1),seeker_(f2){

	}

	reference_type operator*()noexcept{
		return cache_(std::next(prev_));
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

	SlotCallIterator& operator++() {
		++prev_;
		seeker_();
		return *this;
	}
	SlotCallIterator operator++(int) {
		SlotCallIterator old = *this;
		++(*this);
		return old;
	}
};



template<class R,class Iter,class T,class T2>
auto makeSlotIter(const Iter& iter, T& get,T2& seek) {
	return SlotCallIterator<R, T,T2,Iter>{iter, get,seek};
}