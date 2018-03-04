#pragma once
#include "iter_op_detect.h"
#include <type_traits>	//declval
#include "iter_range.h"
#include <algorithm>
#include <vector>
#include "basic_marco_impl.h"

namespace Talg {
	
	/*
		如果键不存在则无副作用发生,并返回false.
	*/
	template<class Map,class Key,class T>
	bool setKey(Map& map,const Key& key,const T& val) {
		auto iter = map.find(key);
		if (iter != map.end()) {
			iter->second = val;
			return true;
		}
		return false;
	}

	template<class Map,class Key,class T>
	decltype(auto) toKey(const Map& map,const Key& key,const T& val) {
		auto iter = map.find(key);
		return iter != map.end()?iter->second:val;
	}

	//template<class Map,class Key>
	//auto toKey(const Map& map,const Key& key) 
	//-> Expected<typename Map::mapped_type> 
	//{
	//	auto iter = map.find(key);
	//	return iter != map.end()?iter->second:Expected<typename Map::mapped_type>{};
	//}


	/*
		如果键已经存在则无副作用发生.
		/had renamed from: addKey<Map,Key,T>(Map&,const Key&,const T&)
	*/
	template<class Map,class Key,class...Ts>
	bool tryInsertKey(Map& map,Key&& key,Ts&&...args) {
		auto iter = map.lower_bound(key);
		if (bool(iter == map.end()) || bool(key < iter->first)) {
			//now: *iter!=key
			map.emplace_hint(iter, forward_m(key),forward_m(args)...);
			return true;
		}
		return false;
	}

	template<class T>
	struct RefEqualPred{
		T ref;
		static_assert(std::is_reference<T>::value, "");
		template<class U>
		explicit constexpr RefEqualPred(ExcludeBase<RefEqualPred<T>,U>&& v)noexcept
			:ref(forward_m(v)){}
		constexpr RefEqualPred(const RefEqualPred<T>& rhs)noexcept
			:ref(std::forward<T>(rhs.ref)){}
		constexpr RefEqualPred(RefEqualPred<T>&& rhs)noexcept
			:ref(std::forward<T>(rhs.ref)){}

		template<class U>
		bool operator()(U&& rhs)
		noexcept(noexcept(std::forward<U>(ref) == forward_m(rhs))) {
			return forward_m(ref) == forward_m(rhs);
		}
		template<class U>
		constexpr bool operator()(U&& rhs)const
		noexcept(noexcept(std::forward<U>(ref) == forward_m(rhs))) {
			return forward_m(ref) == forward_m(rhs);
		}
	};
	template<class T>
	struct ValEqualPred{
		T val_;
		template<class U>
		explicit constexpr ValEqualPred(ExcludeBase<ValEqualPred<T>,U>&& v)noexcept
			:val_(forward_m(v)){}
		constexpr ValEqualPred(const ValEqualPred<T>& rhs)noexcept(std::is_nothrow_copy_constructible<T>::value)
			:val_(rhs.val_){}
		constexpr ValEqualPred(ValEqualPred<T>&& rhs)noexcept(std::is_nothrow_move_constructible<T>::value)
			:val_(std::move(rhs.val_)){}

		template<class U>
		bool operator()(U&& rhs)
		noexcept(noexcept(std::forward<U>(val_) == forward_m(rhs))) {
			return forward_m(val_) == forward_m(rhs);
		}
		template<class U>
		constexpr bool operator()(U&& rhs)const
		noexcept(noexcept(std::forward<U>(val_) == forward_m(rhs))) {
			return forward_m(val_) == forward_m(rhs);
		}
	};

	template<class T>
	constexpr auto predVal(T&& v)noexcept {
		static_assert(std::is_array<std::remove_reference_t<T>>::value==false, 
			"Does not support array now.");
		return ValEqualPred<std::decay_t<T>>{ forward_m(v) };
	}
	template<class T>
	constexpr auto predRef(T&& v)noexcept {
		static_assert(std::is_array<std::remove_reference_t<T>>::value==false,
			"Does not support array now.");
		return RefEqualPred<T&&>{ forward_m(v) };
	}


	template<class U,class Pred=std::less<>>
	void sort(Rag<U>& range,Pred p={}) {
		std::sort(std::begin(range), std::end(range),p);
	}

	template<class U,class Pred>
	bool all_of(ConstRag<U>& rag, Pred&& p) {
		return std::all_of(std::begin(rag), std::end(rag), forward_m(p));
	}

	template<class U,class Pred>
	bool any_of(ConstRag<U>& rag, Pred&& p) {
		return std::any_of(std::begin(rag), std::end(rag), forward_m(p));
	}

	template<class U, class Pred>
	bool none_of(ConstRag<U>& rag, Pred&& p) {
		return std::none_of(std::begin(rag), std::end(rag), forward_m(p));
	}
	

	template<class U, class T>
	auto count(ConstRag<U>& rag, const T &value) -> typename U::difference_type {
		return std::count(std::begin(rag), std::end(rag), value);
	}

	template<class U, class Pred>
	auto count_if(ConstRag<U>& rag, Pred p)-> typename U::difference_type {
		return std::count_if(std::begin(rag), std::end(rag), p);
	}

	template<class U,class Iter>
	void copy(ConstRag<U>& rag, Iter out) {
		std::copy(std::begin(rag), std::end(rag), out);
	}
	template<class U,class T>
	void fill(Rag<U>& rag, const T& val) {
		std::fill(std::begin(rag), std::end(rag), val);
	}
	template<class U,class Size,class T>
	void fill_n(Rag<U>& rag,Size count,const T& val) {
		std::fill_n(std::begin(rag), count, val);
	}

	template<class T,class U,class...Ts>
	constexpr const T& min(const T& lhs,const U& rhs,const Ts&...args) {
		return lhs < rhs ? min(lhs,args...) : min(rhs,args...);
	}
	template<class T,class U,class...Ts>
	constexpr const T& max(const T& lhs,const U& rhs,const Ts&...args) {
		return lhs < rhs ? max(rhs,args...) : max(lhs,args...);
	}
	template<class T>
	constexpr const T& min(const T& lhs) {
		return lhs;
	}
	template<class T>
	constexpr const T& max(const T& lhs) {
		return lhs;
	}

	template<class U,class Iter,class F>
	bool find_if(ConstRag<U>& rag,F func,Iter& out) {
		auto end = std::end(rag);
		auto iter = std::find_if(std::begin(rag),end, func);
		if (iter != end) {
			out = iter;
			return true;
		}
		return false;
	}

	template<class U,class Iter,class T>
	bool find(ConstRag<U>& rag,const T& value,Iter& out) {
		auto end = std::end(rag);
		auto iter = std::find(std::begin(rag),end,value);
		if (iter != end) {
			out = iter;
			return true;
		}
		return false;
	}
	
	template<class U,class Iter,class F>
	bool hasIf(ConstRag<U>& rag,F&& func) {
		for (auto&& elem : rag) {
			if (forward_m(func)(forward_m(elem)))
				return true;
		}
		return false;
	}
	
	template<class U,class Iter,class T>
	bool hasValue(ConstRag<U>& rag,T&& arg) {
		for (auto&& elem : rag) {
			if (forward_m(elem) == forward_m(arg))
				return true;
		}
		return false;
	}
	template<class U,class T>
	bool hasKey(ConstRag<U>& rag,T&& key) {
		for (auto&& elem : rag) {
			if( !(elem.first<key) && !(key<elem.first) )
				return true;
		}
		return false;
	}
}

#include "undef_macro.h"

