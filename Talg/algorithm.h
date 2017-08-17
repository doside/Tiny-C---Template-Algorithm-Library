#pragma once
#include "iter_op_detect.h"
#include <type_traits>	//declval
#include "iter_range.h"
#include <algorithm>

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
	*/
	template<class Map,class Key,class T>
	bool addKey(Map& map,const Key& key,const T& val) {
		auto iter = map.find(key);
		if (iter == map.end()) {
			map[key] = val;
			return true;
		}
		return false;
	}

	


	template<class R,class Pred=std::less<>>
	void sort(const RandomRag<R>& range,Pred p={}) {
		std::sort(std::begin(range), std::end(range),p);
	}

	template<class Rag,class Pred >
	bool all_of(const Rag& rag, Pred p) {
		return std::all_of(std::begin(rag), std::end(rag), p);
	}

	template<class Rag,class Pred >
	bool any_of(const Rag& rag, Pred p) {
		return std::any_of(std::begin(rag), std::end(rag), p);
	}

	template<class Rag, class Pred>
	bool none_of(const Rag& rag, Pred p) {
		return std::none_of(std::begin(rag), std::end(rag), p);
	}

	template<class Rag, class T>
	auto count(const Rag& rag, const T &value) -> typename Rag::difference_type {
		return std::count(std::begin(rag), std::end(rag), value);
	}

	template<class Rag, class Pred>
	auto count_if(const Rag& rag, Pred p)-> typename Rag::difference_type {
		return std::count_if(std::begin(rag), std::end(rag), p);
	}

	template<class Rag,class Iter>
	void copy(const Rag& rag, Iter out) {
		std::copy(std::begin(rag), std::end(rag), out);
	}
	template<class Rag,class T>
	void fill(const Rag& rag, const T& val) {
		std::fill(std::begin(rag), std::end(rag), val);
	}
	template<class Rag,class Size,class T>
	void fill_n(const Rag& rag,Size count,const T& val) {
		std::fill_n(std::begin(rag), count, val);
	}
}


