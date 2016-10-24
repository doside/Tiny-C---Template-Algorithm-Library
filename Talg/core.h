#pragma once

#include <utility>
#include <cstddef>

/*
������Լ��
??��ʾ�����������(����,ģ��(template<class>class)��������,����(size_t,int...)��������)

������׺��ʾ�Բ�������Seqfy,ע��� T ʹ��Seqfy���Ա�֤��Seq<T>,��A<??>����֤��Seq<??>
������׺ʱ�����������������κμٶ� ��ʱ���ܵ�ͬ��_t,����ʱ���ֿ�����Ҫʹ��OMIT_T

_s��ʾ����һ����Seq<??>,�������������ϲ����ܴ��ڷ��ض�����͵Ļ���ô������� T, ��������Seq<??>��Seq<T>
_n��ʾ����::value_type һ����std::index_sequence<...>
_t��ʾ����::type
_v��ʾ����::value(�ֽ׶�����msvc��֧��std::??_v��û��ʹ����������)
_ss��ʾ_s,���⻹һ������һ��Seq<??>
_sv��ʾ����һ������,�������::value�ɹ�ʹ��
_sv��ʾ����һ������,�������::type �ɹ�ʹ��
_svt��ʾ����һ������,�������::value��::type�ɹ�ʹ��

*/










using std::size_t;

#define forward_m(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
#define except_when(...) noexcept(noexcept(__VA_ARGS__))
#define OMIT_T(...) typename __VA_ARGS__::type

template< template<class...> class Dst >
struct TransformImp
{
	template< template<class...>class Src, class...Args >
	static Dst<Args...> from(Src<Args...>&&);

	template<class T, class...Args >
	static Dst<T> from(T&&);

};
template< template<class...> class Dst, class T>
using Transform = decltype(TransformImp<Dst>::from(std::declval<T>()));




template<class T>
struct WrapperT {  //In some context ,we need a wrapper to use T which does't have ctor.
	using type = T;
};
template<class...Args>
struct Seq { };
template<class T>
using Seqfy = Transform<Seq, T>;  //�� blabla<abcde>ת����Seq<abcde>


template<class>struct SeqSize_sv;
template<class...Ts>
struct SeqSize_sv<Seq<Ts...>>:std::integral_constant<size_t , sizeof...(Ts)> {};
template<class T>
using SeqSize = SeqSize_sv<Seqfy<T>>;


template<class...Ts>
constexpr size_t countSeqSize(const Seq<Ts...>&)noexcept {
	return sizeof...(Ts);
}




#define assert_is_seq(...) __VA_ARGS__


template<class...>struct MergeImp;
template<class...Ts, class...Others>
struct MergeImp<Seq<Ts...>, Seq<Others...>>
{
	using type = Seq<Ts..., Others...>;
};
template<class first, class second>
using Merge = OMIT_T(MergeImp<Seqfy<first>, Seqfy<second> >);
template<class T, class U>
using Merge_s = OMIT_T(MergeImp<assert_is_seq(T), assert_is_seq(U)>);


template<class...>struct ReverseImp;

template<class T, class...Ts>
struct ReverseImp<Seq<T, Ts...>> {
	using type = Merge_s<OMIT_T(ReverseImp<Seq<Ts...>>), Seq<T> >;
};
template<class T>
struct ReverseImp<Seq<T>> {
	using type = Seq<T>;
};

template<class T>
using Reverse = OMIT_T(ReverseImp<Seqfy<T>>);
template<class obj>
using Reverse_s = OMIT_T(ReverseImp<assert_is_seq(obj)>);



