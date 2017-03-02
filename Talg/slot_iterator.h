#pragma once
#include "iter_call_cache.h"
#include <functional>
#include <cassert>

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
	bool val_ = false;
public:
	SlotCallIterator(const Iterator& iter,Func& f1,Func2& f2)
	:prev_(iter),cache_(f1),seeker_(f2){
	
	}

	reference_type operator*()noexcept{
		if (val_) {
			return  cache_(prev_);
		}
		reference_type res = cache_(std::next(prev_), false);
		val_ = false;
		return res;
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
		seeker_(prev_);
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