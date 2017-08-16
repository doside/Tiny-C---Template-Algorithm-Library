#pragma once
#include <cstddef>		//ptrdiff_t
#include <iterator>
#include "core.h"
#include "type_traits.h"	//Tself

namespace Talg {


	template<class T>
	using SeqPtr = Talg::Seq<T>*;

	template<class T>
	constexpr bool hasIndexOp(SeqPtr<decltype(std::declval<T&>()[0])>) { return true; } //[]有可能返回void所以用seq包住.
	template<class T>
	constexpr bool hasIndexOp(EatParam) { return false; }

	template< class, class = void_t<> >
	struct hasPreIncOp : std::false_type { };
	template< class T >
	struct hasPreIncOp<T,void_t<decltype( ++std::declval<T&>())>>: std::true_type { };

	template< class, class = void_t<> >
	struct hasPreDecOp : std::false_type { };
	template< class T >
	struct hasPreDecOp<T,void_t<decltype( --std::declval<T&>())>>: std::true_type { };

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

	
	
	
	


	template<class T>
	struct InputRagRequire{
		static constexpr bool value = true;	//todo fix!!
	};
	template<class T>
	struct OutputRagRequire{
		static constexpr bool value = true;	//todo fix!!
	};
	template<class T>
	struct ForwardRagRequire{
		static constexpr bool value = AndValue<InputRagRequire<T>,std::is_default_constructible<T>>{};	
		//todo fix!!
	};
	template<class T>
	struct BidrectedRagRequire{
		static constexpr bool value = true;	//todo fix!!
	};
	template<class T>
	struct RandomRagRequire{
		static constexpr bool value = true;	//todo fix!!
	};

	template<class R>
	using RandomRag = Tself<R, RandomRagRequire<R>>;
	template<class R>
	using ForwardRag = Tself<R, ForwardRagRequire<R>>;
	template<class R>
	using OutputRag = Tself<R, OutputRagRequire<R>>;
	template<class R>
	using InputRag = Tself<R, InputRagRequire<R>>;
	template<class R>
	using BidrectedRag = Tself<R, BidrectedRagRequire<R>>;


}