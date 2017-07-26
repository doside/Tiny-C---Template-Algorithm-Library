#pragma once
#include "core.h"
#include <cstddef>		//ptrdiff_t
#include <iterator>

namespace Talg {
	template<class T>
	using SeqPtr = Talg::Seq<T>*;

	template<class T>
	constexpr bool hasIndexOp(SeqPtr<decltype(std::declval<T&>()[0])>) { return true; } //[]有可能返回void所以用seq包住.
	template<class T>
	constexpr bool hasIndexOp(EatParam) { return false; }

	template<class T>
	constexpr bool hasPreIncOp(SeqPtr<decltype(++std::declval<T&>())>) { return true; }
	template<class T>
	constexpr bool hasPreIncOp(EatParam) { return false; }

	template<class T>
	constexpr bool hasPreDecOp(SeqPtr<decltype(--std::declval<T&>())>) { return true; }
	template<class T>
	constexpr bool hasPreDecOp(EatParam) { return false; }

	template<class T>
	constexpr bool hasSuffixIncOp(SeqPtr<decltype(std::declval<T&>()++)>) { return true; }
	template<class T>
	constexpr bool hasSuffixIncOp(EatParam) { return false; }

	template<class T>
	constexpr bool hasSuffixDecOp(SeqPtr<decltype(std::declval<T&>()--)>) { return true; }
	template<class T>
	constexpr bool hasSuffixDecOp(EatParam) { return false; }

	template<class T>
	constexpr bool hasPointerOp(SeqPtr<decltype(std::declval<T&>().operator->())>) { return true; }
	template<class T>
	constexpr bool hasPointerOp(EatParam) { return false; }

	template<class Iter>
	typename Iter::difference_type getIterDiffType(void* i);
	template<class Iter>
	std::ptrdiff_t getIterDiffType(Talg::EatParam);


	template<class Iter>
	typename Iter::reference getIterRefType(void* i);
	template<class Iter>
	typename Iter::value_type& getIterRefType(Talg::EatParam);

	template<class Iter>
	typename Iter::value_type getIterValType(void* i);

	template<class Iter>
	typename Iter::pointer getIterPtrType(void* i);
	template<class Iter>
	typename Iter::value_type* getIterPtrType(Talg::EatParam);



	template<class Iter>
	typename Iter::iterator_category getIterCategory(void* i);
	template<class Iter>
	auto getIterCategory(Talg::EatParam)->
		std::conditional_t<Talg::hasIndexOp<Iter>(nullptr),
			std::random_access_iterator_tag,
			std::conditional_t<Talg::hasPreIncOp<Iter>(nullptr),
				std::bidirectional_iterator_tag,
				std::conditional_t<Talg::hasPointerOp<Iter>(nullptr),
					std::input_iterator_tag,
					std::output_iterator_tag
				>
			>
		>;


	template<class Iter>
	auto getCrefType(Iter i,Talg::Seq<typename Iter::const_reference>* =nullptr)-> typename Iter::const_reference;
	template<class Iter>
	const typename Iter::value_type& getCrefType(Talg::EatParam);

	template<class T>
	struct IterCrefType {
		using type = decltype(getCrefType<T>(std::declval<T>()));
	};
	template<class T>
	struct IterCrefType<T*> {
		using type = const T&;
	};

	template<class T>
	using IterCrefType_t = typename IterCrefType<T>::type;

	template<class Iter>
	using IterDiff_t = decltype(getIterDiffType<Iter>(nullptr));

	template<class Iter>
	using IterValueType_t = decltype(getIterValType<Iter>(nullptr));

	template<class Iter>
	using IterRefType_t = decltype(getIterRefType<Iter>(nullptr));

	template<class Iter>
	using IterPtrType_t = decltype(getIterPtrType<Iter>(nullptr));

	template<class Iter>
	using IterCategory_t = decltype(getIterCategory<Iter>(nullptr));




}