#pragma once
#include "iter_call_cache.h"
#include <functional>
#include <cassert>

template<class R,class Func,class Iterator>
class SlotCallIterator{
	using reference_type	= typename CacheRes<R>::reference_type;
	using value_type		= typename CacheRes<R>::value_type;
	using pointer			= typename CacheRes<R>::pointer;
	//采用inputiterator,因为允许op++使迭代器失效.并且不是default constructable
	using iterator_category = std::input_iterator_tag;	
	using difference_type	= typename Iterator::difference_type;
	Iterator iter_;
	std::reference_wrapper<Func> cache_;
public:
	SlotCallIterator(const Iterator& iter,Func& data)
	:iter_(iter),cache_(data){
		//cache_(iter_,false);
	}

	decltype(auto) operator*()noexcept{
		return  cache_(iter_).get();
	}
	friend bool operator==(const SlotCallIterator& lhs, const SlotCallIterator& rhs)
	noexcept(noexcept(lhs.iter_==rhs.iter_))
	{
		return lhs.iter_ == rhs.iter_;
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
		++iter_;
		if (iter_->state != nullptr && iter_->state->is_blocked()) {
			iter_ = iter_->state->get_next();
		}
		cache_(iter_,false);
		/*while(iter->state_obser != nullptr){
			auto* state = iter->state_obser;
			if (state->is_free()) {
				break;
			}
			++iter;
		}*/
		return *this;
	}
	SlotCallIterator operator++(int) {
		SlotCallIterator prev = *this;
		++(*this);
		return prev;
	}
};

template<class R,class Iter,class T>
auto makeSlotIter(const Iter& iter,T& lambda) {
	return SlotCallIterator<R, T, Iter>{iter, lambda};
}