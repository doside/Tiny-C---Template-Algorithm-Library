#pragma once
#include "seqop.h"
#include "select_type.h"
#include "utility.h"
#include <iterator>

namespace Talg {

	//用于无视掉参数包的类型而简单地将其视作是T
	template<class T,class U>
	using PackAs = std::conditional_t<true, T, U>;

	struct bad_access:std::logic_error {
		using std::logic_error::logic_error;
	};
	template<class List>
	struct InitListIter{
		typedef std::random_access_iterator_tag iterator_category;
		typedef typename List::value_type value_type;
		typedef std::ptrdiff_t difference_type;
		typedef const value_type *pointer;
		typedef const value_type& reference;
		using Iter = InitListIter;


		size_t id;
		const List& dat;

		constexpr InitListIter(size_t index,const List& data)noexcept
			:id(index),dat(data){}

		Iter& operator++()noexcept {
			++id;
			return *this;
		}
		Iter operator++(int)noexcept{
			Iter val = *this;
			++*this;
			return val;
		}
		Iter& operator--()noexcept {
			--id;
			return *this;
		}
		Iter operator--(int)noexcept{
			Iter val = *this;
			--*this;
			return val;
		}
		Iter& operator+=(difference_type n)noexcept{
			id+=n;
			return *this;
		}
		Iter& operator-=(difference_type n)noexcept{
			return *this+=-n;
		}
			
		constexpr Iter operator+(difference_type n)const noexcept{
			return Iter(id + n, dat);
		}
		friend constexpr Iter operator+(difference_type n,const Iter& rhs)noexcept{
			return rhs + n;
		}
		constexpr Iter operator-(difference_type n)const noexcept{
			return Iter(id - n, dat);
		}
		friend constexpr Iter operator-(const Iter& lhs,const Iter& rhs)noexcept{
			return lhs - rhs.id;
		}

		
		template<size_t I=0,
			class=std::enable_if_t< (I < List::size) >
		>
		constexpr reference get(size_t id)const {
			return I == id ? dat.template get<I>() : this->get<I + 1>(id);
		}

		template<size_t I>
		reference get(size_t,std::enable_if_t<(I >= List::size),int> = 0)const {
			static_assert(I >= List::size, "I is out of range.");
			throw bad_access("InitListIter::get") ; 
		}



		constexpr const value_type& operator*()const noexcept {
			return get(id);
		}
		constexpr const value_type* operator->()const noexcept {
			return &get(id);
		}
		constexpr const value_type& operator[](size_t id)const noexcept {
			return get(id);
		}

		constexpr bool operator!=(const Iter& rhs)const noexcept{
			return id != rhs.id;
		}
		constexpr bool operator==(const Iter& rhs)const noexcept{
			return id == rhs.id;
		}
		constexpr bool operator<(const Iter& rhs)const noexcept{
			return id < rhs.id;
		}
		constexpr bool operator>(const Iter& rhs)const noexcept{
			return id > rhs.id;
		}
		constexpr bool operator<=(const Iter& rhs)const noexcept{
			return id <= rhs.id;
		}
		constexpr bool operator>=(const Iter& rhs)const noexcept{
			return id >= rhs.id;
		}
	};



	template<class Ref>
	struct RefWrapper {
		Ref ref;
		static_assert(std::is_reference<Ref>::value, "RefWrapper require T to be reference.");
		constexpr RefWrapper(Ref r)noexcept:ref(r){}
		constexpr operator Ref()const noexcept { return ref; }
		operator Ref()noexcept { return ref; }
	};

	template<class T,class...Ts>
	struct InitList{
	public:
		std::tuple<const PackAs<T, Ts>&...> dat;

		static constexpr size_t size = sizeof...(Ts);
		using Iter = InitListIter<InitList>;
		using iterator = Iter;
		using const_iterator = Iter;
		using value_type = T;
		using reference = T&;
		using const_reference = const T&;
		using size_type = std::size_t;
		using pointer = T*;
		using const_pointer = const T*;
		using difference_type = std::ptrdiff_t;
	public:
		template<class...Args>
		constexpr InitList(Args&&...args)
		:dat(forward_m(args)...){
			static_assert(staticCheck<AndValue<is_decay_same<T, Args>...> >(), "Each Ts should be the same type.");
		}
		
		template<size_t N>
		constexpr const T& get()const noexcept {
			static_assert(staticCheck< (N < size),IdSeq<N,size> >(), "N out of range");
			return std::get<N>(dat);
		}
		
		constexpr Iter begin()const { return Iter{ 0,*this }; }
		constexpr Iter end()const { return Iter{ size,*this }; }

	};




}
