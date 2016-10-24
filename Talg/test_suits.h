#pragma once
#include <type_traits>

//���뱨��ʱ���Ը���itag<?>��������ǵ�?+1��Υ����is_same,���ҿ��Ը���itag<0>����λ��ĵ��õ�
#define testSame(...) static_assert(AssertIsSame<itag<0>,__VA_ARGS__>::value,"Look here");

///�ڲ�ʹ��
template<size_t n>struct itag{};


//We use those struct to generate more information about static_assert failure. (Especially on msvc)


template<class T, class... Ts>
struct StaticAssert {
	static_assert(T::value, "T::value is not true");
	constexpr StaticAssert() {}
	constexpr operator bool()const {
		return T::value;
	}
};

template<class T>
struct NotValue :std::integral_constant<bool, !T::value> { };


template<class...Ts>
struct AssertIsSame;


//����clang��cl��������ʾ��itag����ȷֵ,��gccȴû��������ʾ�������ʵ����
//���Դ˴���itag����Ϣֱ�Ӳ��뵽������,�������һ���ı��븺��.
template<size_t n,class L, class R, class... Ts>
struct AssertIsSame<itag<n>,L, R, Ts...>: StaticAssert<std::is_same<L, R>,itag<n>>
{
	static constexpr bool value = AssertIsSame<itag<n+1>,Ts...>::value;
};

template<size_t n, class L, class R>
struct AssertIsSame<itag<n>, L, R> :
	std::is_same<L, R>,
	StaticAssert<std::is_same<L, R>,itag<n>>
{

};

template< class L, class R, class... Ts>
struct AssertIsSame< L, R, Ts...> : StaticAssert<std::is_same<L, R>>
{
	static constexpr bool value = AssertIsSame<Ts...>::value;
};

template<class L, class R>
struct AssertIsSame<L, R> : 
	std::is_same<L, R>, 
	StaticAssert<std::is_same<L, R>>
{ 

};




//template<class T>
//using AssertInform = OMIT_T(AssertInformImp<T>);
#define AssertInform(msg,...)  \
static_assert( StaticAssert<__VA_ARGS__>{},msg " "#__VA_ARGS__ );


template<class... Ts>
using NotSame = NotValue<std::is_same<Ts...>>;