#ifndef HAS_MEMBER_H_INCLUDED
#define HAS_MEMBER_H_INCLUDED
#include <type_traits>

template<class L,class R=L>
struct hasEqualCompare{
	template<class Lhs,class Rhs,class =decltype(std::declval<Lhs>()==std::declval<Rhs>())>
	static std::true_type detect(int);
	template<class Lhs,class Rhs>
	static std::false_type detect(...);
	
	static constexpr bool value = decltype(detect<L,R>(0))::value;
};
/*template<class T>
struct hasEqualCompare:decltype(isEqualAbleImp::detect<T>(0))
{

};*/
#endif // !HAS_MEMBER_H_INCLUDED
