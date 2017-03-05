#pragma once
#include <forward_list>
#include <cassert>
namespace Talg{
template<class T,class Allocator = std::allocator<T>>
class SingleList:private std::forward_list<T,Allocator> {
public:
	using Base = std::forward_list<T, Allocator>;
	using Base::Base;
	using value_type		=typename Base::value_type;
	using size_type			=typename Base::size_type;
	using difference_typ	=typename Base::difference_type;
	using reference			=typename Base::reference;
	using const_referenc	=const T&;
	using pointer			=typename Base::pointer;
	using const_pointer		=typename Base::const_pointer;
	using iterator			=typename Base::iterator;
	using const_iterator	=typename Base::const_iterator;
private:
	iterator last;
	bool wasEmpty()const noexcept {
		return last == Base::before_begin();
	}
	void assign(const SingleList& rhs) {
		auto old = last;
		try {
			last = before_begin();
			auto end = rhs.cend();
			for (auto iter =rhs.cbegin();iter!=end; ++iter) {
				//todo fix: thie line require T be copy constructable,but it should be not. 
				last = Base::insert_after(last, *iter);
			}
			Base::erase_after(before_begin(), ++old);
		}
		catch (...) {
			last = old;
			throw;
		}
	}
public:
	using Base::get_allocator;
	using Base::front;
	using Base::before_begin;
	using Base::begin;
	using Base::end;
	using Base::cbefore_begin;
	using Base::cbegin;
	using Base::cend;
	using Base::empty;
	using Base::max_size;
	
	SingleList()
		noexcept(std::is_nothrow_default_constructible<Base>::value)
		:Base(),last(before_begin())
	{}
	SingleList(SingleList&& rhs)noexcept(std::is_nothrow_move_constructible<Base>::value)
		:Base(std::move(rhs)),last(rhs.last){}

	SingleList(const SingleList& rhs) 
	:Base(),last(before_begin())
	{
		assign(rhs);
	}
	
	SingleList& operator=(SingleList&& rhs)
		noexcept(std::is_nothrow_move_assignable<Base>::value) 
	{
		Base::operator=(std::move(rhs));
		last = rhs.last;
		return *this;
	}
	SingleList& operator=(const SingleList& rhs) {
		if (this != &rhs) {
			assign(rhs);
		}
		return *this;
	}
	const_iterator cbefore_end()const noexcept {
		return last;
	}
	const_iterator before_end()const noexcept {
		return last;
	}
	iterator before_end()noexcept {
		return last;
	}
	reference back()noexcept {
		return *last;
	}
	const T& back() const noexcept{
		return *last;
	}
	
	void push_back(const T& value) {
		last=Base::insert_after(last, value);
	}
	void push_back(T&& value) {
		last=Base::insert_after(last, std::move(value));
	}
	
	iterator insert_after(const_iterator pos, const T& value) {
		if(pos!=last){
			return Base::insert_after(last, value);
		} else {
			push_back(value);
			return last;
		}
	}
	iterator insert_after(const_iterator pos, T&& value) {
		if(pos!=last){
			return Base::insert_after(last, std::move(value));
		} else {
			push_back(std::move(value));
			return last;
		}
	}
	void push_front(const T& value) {
		Base::push_front(value);
		if (wasEmpty()) {
			last = begin();
		}
	}
	void push_front(T&& value) {
		Base::push_front(std::move(value));
		if (wasEmpty()) {
			last = begin();
		}
	}
	

	template<class...Ts>
	reference emplace_front(Ts&&... args) {
		auto res = Base::emplace_front(std::forward<Ts>(args)...);
		if (wasEmpty()) {
			last = res;
		}
		return *res;
	}
	template< class... Ts >
	reference emplace_back(Ts&&... args) {
		auto iter = Base::emplace_after(last, std::forward<Ts>(args)...);
		last = iter;
		return *iter;
	}

	void clear()noexcept {
		Base::clear();
		last = before_begin();
	}
	void pop_front()
		noexcept(noexcept(std::declval<Base>().pop_front())) 
	{
		if (last == begin()) {
			Base::pop_front();
			last = this->before_beign();
		} else {
			Base::pop_front();
		}
	}
	iterator erase_after(iterator pos)
		noexcept(noexcept(std::declval<Base>().erase_after(pos)))
	{
		assert(pos != last && !empty());
		
		auto res = Base::erase_after(pos);
		if (res == end()) {
			last = pos;//Base::erase_after(pos,pos); //为了把const_iterator转换为iterator
		}
		return res;
	}
	iterator clear_before() {
		return Base::erase_after(begin(), last);
	}
	void remove(const T& value) {
		using std::addressof;
		auto iter = begin();
		auto prev = this->before_begin();
		auto tmp = end();
		while (iter != end()) {
			if (*iter == value) {
				if (addressof(*iter) == addressof(value)) {
					tmp = prev;
					++iter;
					++prev;
				} else {
					iter=this->erase_after(prev);
				}
			} else {
				++iter;
				++prev;
			}
		}
		if (tmp != end()) {
			this->erase_after(tmp);
		}
	}
	template< class UnaryPredicate >
	void remove_if(UnaryPredicate pred) {
		using std::addressof;
		auto prev = this->before_begin();
		auto iter = begin();
		while (iter != end()) {
			if (pred(*iter)){
				iter=this->erase_after(prev);
			} else {
				++iter;
				++prev;
			}
		}
	}
	//using Base::assign;
	//using Base::swap;
	//using Base::clear;
	//using Base::remove;
	//using Base::remove_if;
	//using Base::erase_after;
	//using Base::insert_after;
	//using Base::emplace_after;
};

}//namespace Talg
