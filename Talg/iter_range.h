#pragma once
#include <iterator> //for std::iterator_traits
#include <type_traits>
#include "basic_marco_impl.h"

namespace Talg {
	
	template<class Iter>
	class Range{
	public:
		Iter first, last;
		using value_type		= typename std::iterator_traits<Iter>::value_type;
		using pointer			= typename std::iterator_traits<Iter>::pointer;
		using reference			= typename std::iterator_traits<Iter>::reference;
		using difference_type	= typename std::iterator_traits<Iter>::difference_type; //IterDiff_t<Iter>;
		using iterator			= Iter;
	public:
		constexpr Range(Iter&& first_,Iter&& last_)noexcept(std::is_nothrow_move_constructible<Iter>::value)
			:first(std::move(first_)),last(std::move(last_)){}
		constexpr Range(const Iter& first_,const Iter& last_)noexcept(std::is_nothrow_copy_constructible<Iter>::value)
			:first(first_),last(last_){}

		template<class OtherRag>
		constexpr Range(OtherRag&& seq):first(std::begin(forward_m(seq))),last(std::end(forward_m(seq))){}

		constexpr Range(Range&& rag)noexcept(std::is_nothrow_move_constructible<Iter>::value)
		:first(std::move(rag.first)),last(std::move(rag.last)){}

		constexpr Range(const Range& rag)noexcept(std::is_nothrow_copy_constructible<Iter>::value)
			:first(rag.first),last(rag.last){}

		constexpr iterator begin()const {
			return first;
		}
		constexpr iterator end()const {
			return last;
		}
	};

	template<class U>
	using Rag = U;

	template<class U>
	using ConstRag = U;

	template<class Iter>
	auto range(Iter beg, Iter end) { return Range<Iter>(beg, end); }

	template<class Container>
	auto range(Container&& rag) {
		return Range<typename Container::iterator>(forward_m(rag));
	}
}
#include "undef_macro.h"
