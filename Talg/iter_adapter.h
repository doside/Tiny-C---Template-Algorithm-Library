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
}



