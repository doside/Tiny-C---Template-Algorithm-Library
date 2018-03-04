#pragma once
#include "core.h"
#include <utility>
#include <limits>
#include "basic_marco_impl.h"

//using std::size_t;
namespace Talg{
enum :size_t {
	no_index = std::numeric_limits<size_t>::max() //抑制VC的警告C4307max() - 1
};
struct NoType :public Seq<NoType> {};

template<size_t... nums>
using IdSeq = std::index_sequence<nums...>;

template<size_t beg,size_t count>
constexpr auto makeIdSeq() { return makeIdSeq<beg>(std::make_index_sequence<count>()); }

template<size_t beg,size_t...nums>
constexpr auto makeIdSeq(IdSeq<nums...>) {
	return IdSeq<beg + nums...>();
}

template<
	template<size_t...>class Dst,
	template<size_t...>class Src,
	size_t...I,
	size_t...I2
>
Dst<I...> IdSeqTrans(Dst<I...>&, Src<I2...>&);

template<class Dst,class Src>
using IdSeqTransform = decltype(IdSeqTrans(std::declval<Dst&>(), std::declval<Src&>()));



/*
如果自己实现一个IdSeq的话,还要写个makeIdSeq<N>(),
这种设施的高效实现比较复杂,并且有可能是直接由编译器内建从而加快编译速度的,
此外如果两者不同的话, 用户混用了index_sequence和IdSeq会使得产生不必要的实例化
干脆还是用std算了
template<size_t... nums>
struct IdSeq {
	using value_type = size_t;

	int no_used=0;
	//constexpr IdSeq(int){}
	//constexpr IdSeq(){}
	constexpr IdSeq(int ){}
	constexpr IdSeq(){}
	static constexpr std::size_t size() {
		return sizeof...(nums);
	}
};
*/

/*
	\brief	一个将数字绑定到类型的模板类
*/
template<size_t n>
using Tagi = std::integral_constant<size_t,n>;



template<class>struct IndexfyImp;

template<size_t...Ns>
struct IndexfyImp<Seq<IdSeq<Ns>>...> {
	using type = IdSeq<Ns...>;
};
template<class T>
using Indexfy = omit_t_m(IndexfyImp<T>);

template<class T,class U>
struct MergeIndexImp;

template<size_t...Ns,size_t...Ms>
struct MergeIndexImp<IdSeq<Ns...>, IdSeq<Ms...>> {
	using type = IdSeq<Ns..., Ms...>;
};


template<class T,class U>
using MergeIndex = omit_t_m(MergeIndexImp<T, U>);
template<class T, class U>
using MergeIndex_s = omit_t_m(MergeIndexImp<T, U>);

template<size_t...Ns>
Seq<Tagi<Ns>...> makeTagiSeq(IdSeq<Ns...>&&);

template<size_t N>
using TagiSeqN = decltype(makeTagiSeq(std::make_index_sequence<N>()));


}//namespace Talg
#include "undef_macro.h"
