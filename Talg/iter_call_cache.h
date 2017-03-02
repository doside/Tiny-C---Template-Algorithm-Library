#pragma once
#include <type_traits>
#include <memory>	//for std::addressof
#include "core.h"

template <class T>
class RefWrapper {
	T* ptr=nullptr;
public:
	static_assert(std::is_reference<T>::value, "RefWrapper should be used for reference only.");
	using type=T;
	
	RefWrapper() = default;
	RefWrapper(T ref) noexcept : ptr(std::addressof(ref)) {}
	RefWrapper(const RefWrapper&) noexcept = default;
	RefWrapper(RefWrapper&&) noexcept = default;
	
	RefWrapper& operator=(const RefWrapper& x) noexcept = default;
	RefWrapper& operator=(RefWrapper&& x) noexcept = default;
	
	operator T () const noexcept { return forward_m(*ptr); }
	T get() const noexcept { return forward_m(*ptr); }
	void reset(T ref) {
		ptr = &ref;
	}
};
template<class T>
struct OptionalVal {
	union {
		T val;
		//bool dummy;
	}val;
	//bool is_active=false;
	OptionalVal()noexcept{}
	void operator=(OptionalVal) = delete;
	OptionalVal(OptionalVal&&) = delete;

	template<class Iter,class...Ts>
	void reset(const Iter& iter,Ts&&...args) {
		//if(!is_active)
		val.val.~T();
		new(&(val.val)) T( (*iter)(forward_m(args)...) );
	}
	
	//explicit operator bool()const noexcept {
	//	return is_active;
	//}

	const T& get()const noexcept{
		return val.val;
	}
	T& get()noexcept{
		return val.val;
	}
	~OptionalVal() {
		val.val.~T();
	}
};


template<class R>
struct CacheRes:public OptionalVal<R> {
	 //todo: we shall use a std::optional<R> instead of unique_ptr.
	using Base = OptionalVal<R>;
	using reference_type	= R&;
	using value_type		= R;
	using pointer			= R*;
};

template<>
struct CacheRes<void>{
	using reference_type	= const CacheRes<void>&;
	using value_type		= const CacheRes<void>;
	using pointer			= const CacheRes<void>*;
	template<class Iter,class...Ts>
	void reset(const Iter&iter,Ts&&...args)const { 
		(*iter)(forward_m(args)...); 
	}
	reference_type get()const noexcept{
		return *this;
	}
	CacheRes& operator=(CacheRes<void>&&) = delete;
	CacheRes(CacheRes<void>&&) = delete;
};
template<class R>
class CacheRes<R&&>: public RefWrapper<R&&>{ };












template<class T>
struct OptionalVal_safe {
	union {
		T val;
		bool dummy;
	}val;
	bool is_active=false;
	OptionalVal_safe() = default;
	void operator=(OptionalVal_safe) = delete;
	OptionalVal_safe(OptionalVal_safe&&) = delete;

	template<class Iter,class...Ts>
	void reset(const Iter&iter,Ts&&...args) { 
		if(!is_active)
			val.val.~T();
		auto* ptr = &(val.val);
		iter->lock_then_call(
			[ptr,&args...](auto&& slot) {
				new(ptr) T( forward_m(slot)(forward_m(args)...) );
			}
		);
		is_active = true;
	}
	
	explicit operator bool()const noexcept {
		return is_active;
	}
	void reset()noexcept {
		if(is_active)
			val.val.~T();
	}
	const T& get()const noexcept{
		return val.val;
	}
	T& get()noexcept{
		return val.val;
	}
	~OptionalVal_safe() {
		reset();
	}
};

template<class R>
struct CacheRes_safe:public OptionalVal<R> {
	 //todo: we shall use a std::optional<R> instead of unique_ptr.
	using Base = OptionalVal<R>;
	using reference_type	= R&;
	using value_type		= R;
	using pointer			= R*;
};

template<>
struct CacheRes_safe<void>{
	using reference_type	= const CacheRes_safe<void>&;
	using value_type		= const CacheRes_safe<void>;
	using pointer			= const CacheRes_safe<void>*;
	bool is_called=false;
	template<class Iter,class...Ts>
	void reset(const Iter&iter,Ts&&...args) { 
		iter->lock_then_call(
			[&args...](auto&& slot) {
				forward_m(slot)(forward_m(args)...); 
			}
		);
		is_called = true;
	}
	void reset()noexcept{
		is_called = false;
	}
	explicit operator bool()const noexcept {
		return is_called;
	}
	const CacheRes_safe<void>& get()const noexcept{
		return *this;
	}
};
template<class R>
class CacheRes_safe<R&&>: public RefWrapper<R&&>{ };
