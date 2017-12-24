#pragma once
#include "iter_op_detect.h"
namespace Talg {

	template<class Iter>
	class IterAdapter{
		using Derived = Iter;
	public:
		using value_type		= IterValue_t<Derived>;
		using reference			= IterRef_t<Derived>;
		using pointer			= IterPtr_t<Derived>;
		using difference_type	= IterDiff_t<Derived>;
		using iterator_category = IterCategory_t<Derived>;
	public:
		decltype(auto) operator*()const noexcept(noexcept(*std::declval<Iter&>())) {
			return *static_cast<Iter&>(*this);
		}
		decltype(auto) operator->()const noexcept(noexcept(&*std::declval<Iter&>())) {
			return &**this;
		}
		Derived& operator++() {
			++static_cast<Iter&>(*this);
			return static_cast<Derived>(*this);
		}
		Derived& operator--() {
			--static_cast<Iter&>(*this);
			return static_cast<Derived>(*this);
		}
		auto operator++(int) {
			Derived iter = static_cast<Derived>(*this);
			++*this;
			return iter;
		}
		auto operator--(int) {
			Derived iter = static_cast<Derived>(*this);
			--*this;
			return iter;
		}
		friend bool operator==(const IterAdapter& lhs, const IterAdapter& rhs) {
			return static_cast<const Iter&>(lhs) == static_cast<const Iter&>(rhs);
		}
		friend bool operator!=(const IterAdapter& lhs, const IterAdapter& rhs) {
			return !(lhs == rhs);
		}

		decltype(auto) operator[](difference_type n)const{
			return *(*this + n);
		}

		Derived& operator+=(difference_type n){
			return static_cast<Iter&>(*this)+=n;
		}
		Derived& operator-=(difference_type n){
			return static_cast<Iter&>(*this)+=-n;
		}
			
		auto operator+(difference_type n)const {
			Derived it = static_cast<Iter&>(*this);
			return it += n;
		}
		auto operator-(difference_type n)const {
			Derived it = static_cast<Iter&>(*this);
			return it -= n;
		}
	
		friend Derived operator+(difference_type n,const Derived& rhs){
			return rhs + n;
		}
		friend difference_type operator-(const Derived& lhs,const Derived& rhs){
			return static_cast<const Iter&>(lhs) - static_cast<const Iter&>(rhs);
		}
		friend bool operator<(const IterAdapter& lhs,const IterAdapter& rhs){
			return static_cast<const Iter&>(lhs) < static_cast<const Iter&>(rhs);
		}
		friend bool operator>(const IterAdapter& lhs,const IterAdapter& rhs){
			return rhs < lhs;
		}
		friend bool operator<=(const IterAdapter& lhs,const IterAdapter& rhs){
			return !(lhs > rhs);
		}
		friend bool operator>=(const IterAdapter& lhs,const IterAdapter& rhs){
			return !(lhs < rhs);
		}
	};

	template<class T>
	[[deprecated]]struct IdentityIter:IterAdapter<IdentityIter<T>> {
		T* ref_=nullptr;
		using value_type		= T;
		using reference			= T&;
		using pointer			= T*;
		using difference_type	= std::ptrdiff_t;
		using iterator_category = std::random_access_iterator_tag;

		constexpr IdentityIter()noexcept{}
		constexpr IdentityIter(T* ptr)noexcept:ref_(ptr){}
		constexpr IdentityIter(T& ptr)noexcept:ref_(&ptr){}

		T& operator*() noexcept{ return *ref_; }
		constexpr const T& operator*()const noexcept{ return *ref_; }
		IdentityIter& operator++() noexcept{
			return *this;
		}
		IdentityIter& operator+=(difference_type n) noexcept{
			return *this;
		}
		bool operator==(const IdentityIter& rhs)const noexcept{ 
			if(ref_!=nullptr&&rhs.ref_!=nullptr)
				return true; 
			return false;
		}
		bool operator<(const IdentityIter& rhs)const noexcept{ 
			return rhs.ref_ && ref_ == nullptr;
		}
		explicit operator bool()const noexcept{
			return ref_ != nullptr;
		}

		T& operator[](difference_type )noexcept { return *ref_; }
		const T& operator[](difference_type )const noexcept{ return *ref_; }
	};


}



