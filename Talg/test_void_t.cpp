#include "type_traits.h"
#include "core.h"
#include <type_traits>

using namespace Talg;

template< class, class = void_t<> >
struct hasPreIncOp : std::false_type { };
template< class T >
struct hasPreIncOp<T,void_t<decltype( ++std::declval<T&>())>>: std::true_type { };

template< class, class = void_t<> >
struct hasPreDecOp : std::false_type { };
template< class T >
struct hasPreDecOp<T,void_t<decltype( --std::declval<T&>())>>: std::true_type { };


namespace {
	
	struct A {
		void operator++(){}
	};
	struct B{
	};

	void f() {
		static_assert(has_pre_increment_member<A>::value, "!");
		static_assert(!has_pre_increment_member<B>::value, "!");
	}

}
