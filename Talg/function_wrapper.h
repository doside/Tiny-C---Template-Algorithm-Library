#ifndef FUNCTION_WRAPPER_H_INCLUDED
#define FUNCTION_WRAPPER_H_INCLUDED

#include "callable_traits.h"
#include "strip_qualifier.h"
#include "find_val.h"
#include "select_type.h"
#include "has_member.h"
#include <type_traits>	
#include <tuple>
#include <memory>
#include "basic_marco_impl.h"

namespace Talg{




template<class T,class S>
struct FindParam {
private:
	using Id1 = Find_if_svt<ParamMatch, T, S>;
	using Id2 = Find_if_svt<ParamConvertMatch, T, S>;
public:
	static constexpr size_t value =Id1::value!=no_index?Id1::value:Id2::value;
	using type = std::conditional_t<Id1::value != no_index,typename Id1::type,typename Id2::type>;
};



/*
	\brief	辅助类型,用特化来延迟类型生成,直接使用conditional的话无法达成此效果.
	\param	T将被适配的用户函子,StandarType标准回调类型,参见FunctorImp
	\return	参数的索引号,例如 IdSeq<0,2,1,3>
*/
template<class T,class StandarType,bool is_direct_invoke>
struct MapParamIdImp{
	using UserFunParam = typename CallableTraits<T>::arg_type;
	using StandarParam = typename CallableTraits<StandarType>::arg_type;
	using type = std::conditional_t<std::is_same<UserFunParam,StandarParam>::value,
		Seq<>,
		Multiop_n<FindParam,UserFunParam,StandarParam>
	>;
};
template<class T,class U>
struct MapParamIdImp<T,U,true>{
	using type = Seq<>;
};
template<class T,class StandarType>
using MapParamId = typename MapParamIdImp<T, StandarType,
		std::is_same<T, StandarType>::value || !isNonOverloadFunctor<T>::value
		//在is_same成立时直接invoke节省编译时间,
		//在没有可能推断出参数类型时(比如一个函数对象有多个operator())也只能直接invoke
	>::type;





/*
	\brief	把T类型的函数对象适配成StandarType,并且提供相等比较.
	\param	T 原函子,StandarType要适配成的类型
	\return	可以像调用StandarType那样调用T
	\note	T的参数必须比StandarType的参数个数少.
			T的参数类型与StandarType的参数类型必须在decay后完全一致,
			并且后者必须可以转换成前者(因为是以StandarType的参数调用前者)
			关于相等运算的语义参见\operator==
*/
template<class T, class StandarType>
struct FunctorImp:private std::tuple<T>{	//派生有助于空基类优化,但由于T可能是基础类型,无法直接从其派生
	using Base = std::tuple<T>;	
	static_assert(!std::is_reference<T>::value, "we assume T is not a ref.");
	using ParameterIndex = MapParamId<T, StandarType>;
	constexpr const T& getFunc()const noexcept{
		return std::get<0>(*this);
	}
	T& getFunc()noexcept{
		return std::get<0>(*this);
	}
public:
	constexpr FunctorImp(const T& f)noexcept(std::is_nothrow_copy_constructible<T>::value)
		:Base(f)
	{

	}
	constexpr FunctorImp(T&& f)noexcept(std::is_nothrow_move_constructible<T>::value)
		: Base(std::move(f))
	{

	}

	template<class...Ts>
	decltype(auto) operator()(Ts&&...args)
		noexcept(noexcept(
			apply(ParameterIndex{},std::declval<T>(), forward_m(args)...)
		))
	{
		//StaticAssert<NotSame<ParameterIndex, IdSeq<>>, T, ParameterIndex,CallableTraits<T>> {};
		return apply(ParameterIndex{},getFunc(), forward_m(args)...);
	}


	template<class...Ts>
	constexpr decltype(auto) operator()(Ts&&...args)const
		noexcept(noexcept(
			apply(ParameterIndex{},std::declval<T>(), forward_m(args)...)
		))
	{
		return apply(ParameterIndex{}, getFunc(), forward_m(args)...);
	}
	

	/*
		\brief	相等比较
		\note	当用户类型提供有相等比较(必须支持const)时,则采用它,
				否则直接根据类型的异同进行判断,如果两者的StandarType不同则引发编译错误.
				
		\todo	如果用户类型是std::function是否应该比较target？
				当用户类型提供有相等比较时,是否应该允许与非StandarType的函数对象比较？
	*/
	template<class U,
		class= std::enable_if_t< hasEqualCompare<const T&,const U&>::value >
	>
	constexpr bool operator==(const FunctorImp<U,StandarType>& rhs)const
		except_when_m(std::declval<T>()== std::declval<U>())
		//->decltype(std::declval<const T&>()== std::declval<const U&>())
	{

		return	getFunc() == rhs.getFunc();
	}

	template<class F,class S>
	constexpr bool operator==(const FunctorImp<F,S>& )const noexcept
	{
		return std::is_same<T, F>::value && std::is_same<StandarType, S>::value;
	}

	template<class F,class S>
	constexpr bool operator!=(const FunctorImp<F,S>& rhs)const
		noexcept(noexcept(std::declval<const FunctorImp&>().operator==(rhs)))
	{
		return !operator==(rhs);
	}

};



template<class ObjPtr,class MemPtr,class StandarT>
struct MemFun
{
	using ParameterIndex = MapParamId<MemPtr, StandarT>;

	//我们在实现中假定了它们是指针,所以有这个断言
	static_assert(std::is_pointer<ObjPtr>::value && 
					(std::is_pointer<MemPtr>::value||
						std::is_member_pointer<MemPtr>::value||
							std::is_same<std::decay_t<MemPtr>,decltype(nullptr)>::value),
					"MemFun static assert failed.");
	ObjPtr ptr_;
	MemPtr pmd;	//todo: pmd可以是自由函数,未必需要限制为成员函数指针
	constexpr MemFun(ObjPtr obj,MemPtr ptr):ptr_(forward_m(obj)),pmd(ptr){}


	template<class...Ts>
	constexpr decltype(auto) operator()(Ts&&...args)const{
		return applyMemFun(ParameterIndex{},ptr_, pmd, forward_m(args)...);
	}

	template<class...Ts>
	decltype(auto) operator()(Ts&&...args) {
		return applyMemFun(ParameterIndex{},ptr_, pmd, forward_m(args)...);
	}


	/*
		\brief	比较相等,用于支持删除相等的函数对象
		\return	如果对象指针为空则比较函数指针,如果函数指针为空则比较对象指针,
				否则同时比较两者.
		\note	ptr_及pmd非空是前置条件,其确保了相等具有对称性.
				不满足传递性
	*/
	template<class Other,class DataU,class S>
	bool operator==(const MemFun<Other, DataU,S>& rhs)const {
		assert(ptr_ != nullptr && pmd!=nullptr);

		if (rhs.ptr_ == nullptr) {
			return pmd == rhs.pmd;
		} else if (rhs.pmd == nullptr) {
			return ptr_ == rhs.ptr_;
		} else {
			return ptr_ == rhs.ptr_ && pmd == rhs.pmd;
		}
	}
};


template<class T,class MemPtr,class StandarT>
class MemFun<std::weak_ptr<T>,MemPtr,StandarT>
{
	MemFun<T*, MemPtr, StandarT> func;
	std::weak_ptr<T> ptr_;
public:
	using ParameterIndex = MapParamId<MemPtr, StandarT>;
	//我们在实现中假定了它们是指针,所以有这个断言
	static_assert(std::is_pointer<MemPtr>::value || std::is_member_pointer<MemPtr>::value,
					"MemFun static assert failed.");
	constexpr MemFun(const MemFun<T*, MemPtr, StandarT>& f):func(f),ptr_(){}

	template<class P>
	MemFun(std::shared_ptr<T> ptr, P&& func_ptr)
		:func(ptr.get(),func_ptr),ptr_(ptr){
		//此处故意用get()保存一个指针,永远不会使用它,
		//但是它的非空让相等比较的对称性成立.
		assert(func.ptr_ != nullptr);
	}


	/*
		\brief	以ptr及其他参数 调用函数(或成员函数)指针pmd,如Functor一样支持可选参数调用
		\param	若干参数,参数列表过长会在ParameterIndex的实例化时引发静态断言.
				如果MapParamId无法推断出来,那么所有参数将直接传递到pmd
		\return	如果MapParamId无法推断出来,则相当于invoke(pmd,ptr_,args...)
				如果MapParamId能推断出id... 则相当于invoke(pmd,ptr_,args<id>...)
		\note	由于必须返回一个值,所以此处没有检查,有扔出异常的可能.
	*/
	template<class...Ts>
	decltype(auto) operator()(Ts&&...args) {
		std::shared_ptr<T> p(ptr_);
		return applyMemFun(ParameterIndex{},p, func.pmd, forward_m(args)...);
	}
	template<class...Ts>
	decltype(auto) operator()(Ts&&...args)const{
		std::shared_ptr<T> p(ptr_);
		return applyMemFun(ParameterIndex{},p, func.pmd, forward_m(args)...);
	}


	/*
		\brief	比较函数,用于支持删除查找.
				不使用友元原本是为了暗示不对称性(现已修正为对称).
		\return	同MemFun
		\note	存在抛出异常的可能性,假定过期的weak_ptr不可比较.
				无视other的weak_ptr成员,以其func成员来和当前对象作比较.
	*/
	bool operator==(const MemFun& other)const {
		const MemFun<T*, MemPtr, StandarT>& rhs = other.func;

		assert(func.ptr_ != nullptr);
		//因为|| func.ptr_==nullptr相当于 ||false,所以此处的条件判定是具有对称性的.
		if (rhs.ptr_ == nullptr) {  
			return func.pmd == rhs.pmd;
		}
		std::shared_ptr<T> sptr(ptr_);
		if (rhs.pmd == nullptr || func.pmd==nullptr ) {
			return sptr == rhs.ptr_;
		} else { 
			return sptr == rhs.ptr_ && func.pmd==rhs.pmd;
		}
	}
};

template<class T,class StandarType>
using SelectFunctorWrapper = std::conditional_t<
	std::is_same<RemoveCvrp<T>, RemoveCvrp<StandarType>>::value,// || !isNonOverloadFunctor<T>::value,
	T,
	FunctorImp<std::remove_reference_t<T>,StandarType>
>;

/*
	\brief	如果可以分析函子的参数表,那么就提供StandarType到其的转化,否则直接保存.
	\param	StandarType标准的回调类型,T需要被调整以适应标准型的函数对象.
	\return 返回适当调整后的函数对象,它符合标准回调类型,并且可比较相等.
	\note	T的参数个数必须比标准型少,少掉的参数视作忽略.
	\todo	提供对成员函数指针的特别支持.
*/
template<class StandarType, class T>
constexpr SelectFunctorWrapper<T&&,StandarType> makeFunctor(T&& src_func) {
	return forward_m(src_func);
}
template<class StandarType,class R,class...Ps>
constexpr SelectFunctorWrapper<R (*)(Ps...),StandarType> makeFunctor(R (*src_func)(Ps...)) {
	return src_func;
}

template<class StandarType, class T,class MemPtr>
constexpr decltype(auto) makeFunctor(T* obj,MemPtr pmd) {
	return MemFun<T*,MemPtr,StandarType>{obj, pmd};
}


template<class StandarType,class T,class MemPtr>
decltype(auto) makeFunctor(std::shared_ptr<T> ptr,MemPtr pmd) {
	return	MemFun<std::weak_ptr<T>,MemPtr,StandarType>{ptr, pmd};
}


}////namespace Talg

#include "undef_macro.h"

#endif //FUNCTION_WRAPPER_H_INCLUDED


