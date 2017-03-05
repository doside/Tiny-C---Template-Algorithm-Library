#ifndef PRIVATE_INVOKE_IMPL_H_INCLUDED
#define PRIVATE_INVOKE_IMPL_H_INCLUDED


#include <type_traits>
#include <functional>
#include <utility>

/*
#if defined(_MSC_VER)
	#if _MSC_VER>=1900
	using std::invoke;
	#endif
#elif  defined(__cpp_lib_invoke)
using std::invoke;
#else
**/
namespace Talg{

namespace impl {

	//以下代码主要复制自 http://en.cppreference.com/w/cpp/utility/functional/invoke possible implement
	//有少许改动.

	template <class T>
	struct is_reference_wrapper : std::false_type {};
	template <class U>
	struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};


	template <class Base, class T, class Derived, class... Args>
	constexpr auto INVOKE(T Base::*pmf, Derived&& ref, Args&&... args)
	noexcept(noexcept((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...)))
		-> std::enable_if_t<
			std::is_function<T>::value && std::is_base_of<Base, std::decay_t<Derived>>::value,
			decltype((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...))
		>
	{
		return (std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...);
	}

	template <class Base, class T, class RefWrap, class... Args>
	constexpr auto INVOKE(T Base::*pmf, RefWrap&& ref, Args&&... args)
	noexcept(noexcept((ref.get().*pmf)(std::forward<Args>(args)...)))
		-> std::enable_if_t<
			std::is_function<T>::value && is_reference_wrapper<std::decay_t<RefWrap>>::value,
			decltype((ref.get().*pmf)(std::forward<Args>(args)...))
		>
	{
		return (ref.get().*pmf)(std::forward<Args>(args)...);
	}

	template <class Base, class T, class Pointer, class... Args>
	constexpr auto INVOKE(T Base::*pmf, Pointer&& ptr, Args&&... args)
	noexcept(noexcept(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...)))
		-> std::enable_if_t<
			std::is_function<T>::value 
			&&!is_reference_wrapper<std::decay_t<Pointer>>::value 
			&&!std::is_base_of<Base, std::decay_t<Pointer>>::value,
			decltype(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...))
		>
	{
		return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
	}

	template <class Base, class T, class Derived>
	constexpr auto INVOKE(T Base::*pmd, Derived&& ref)
	noexcept(noexcept(std::forward<Derived>(ref).*pmd))
		-> std::enable_if_t<
			!std::is_function<T>::value && std::is_base_of<Base, std::decay_t<Derived>>::value,
			decltype(std::forward<Derived>(ref).*pmd)
		>
	{
		return std::forward<Derived>(ref).*pmd;
	}

	template <class Base, class T, class RefWrap>
	constexpr auto INVOKE(T Base::*pmd, RefWrap&& ref)
	noexcept(noexcept(ref.get().*pmd))
		-> std::enable_if_t<
			!std::is_function<T>::value && is_reference_wrapper<std::decay_t<RefWrap>>::value,
			decltype(ref.get().*pmd)
		>
	{
		return ref.get().*pmd;
	}

	template <class Base, class T, class Pointer>
	constexpr auto INVOKE(T Base::*pmd, Pointer&& ptr)
	noexcept(noexcept((*std::forward<Pointer>(ptr)).*pmd))
		-> std::enable_if_t<
			!std::is_function<T>::value 
			&& !is_reference_wrapper<std::decay_t<Pointer>>::value 
			&& !std::is_base_of<Base, std::decay_t<Pointer>>::value,
			decltype((*std::forward<Pointer>(ptr)).*pmd)
		>
	{
		return (*std::forward<Pointer>(ptr)).*pmd;
	}

	template <class F, class... Args>
	constexpr auto INVOKE(F&& f, Args&&... args)
	noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
		-> std::enable_if_t<
			!std::is_member_pointer<std::decay_t<F>>::value,
			decltype(std::forward<F>(f)(std::forward<Args>(args)...))
		>
	{
		return std::forward<F>(f)(std::forward<Args>(args)...);
	}
} // namespace detail


/*
	\brief	std::invoke的等价物,保证了constexpr,
	\note	标准不要求std::invoke支持constexpr
*/
template< class F, class... ArgTypes >
constexpr decltype(auto) ct_invoke(F&& f, ArgTypes&&... args)
noexcept(noexcept(impl::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...)))
{
	return impl::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...);
}

//#endif // !__cpp_lib_invoke


}//namespace Talg

#endif // !PRIVATE_INVOKE_IMPL_H_INCLUDED

