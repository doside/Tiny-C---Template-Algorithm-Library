#ifndef HAS_MEMBER_H_INCLUDED
#define HAS_MEMBER_H_INCLUDED
#include <type_traits>
#include "core.h"
namespace Talg{
template<class L,class R=L>
struct hasEqualCompare{
	template<class Lhs,class Rhs,class =decltype(std::declval<Lhs>()==std::declval<Rhs>())>
	static std::true_type detect(int);
	template<class Lhs,class Rhs>
	static std::false_type detect(...);
	
	static constexpr bool value = decltype(detect<L,R>(0))::value;
};


template<class T>
struct hasOpCall{
	template<class F,class=decltype(& F::operator() )>
	static std::true_type detect(int);
	template<class>
	static std::false_type detect(...);
	static constexpr bool value = decltype(detect<T>(0))::value;
};
template<class T>
using EnableIfT = std::enable_if_t<T::value>;


/*template<class T>
struct hasEqualCompare:decltype(isEqualAbleImp::detect<T>(0))
{

};*/

//template<class F,class StandarT>
//auto getFunctionTraits(F&& func, StandarT* = &func::operator());
}//namespace Talg

#endif // !HAS_MEMBER_H_INCLUDED
