#pragma once
#include <type_traits>
#include <memory>	//for std::addressof
#include "core.h"
#include "basic_marco_impl.h"

#if 0
template<bool is_void=true>
struct lock_then_call {
	template<class Func,class Iter>
	void operator()(const Iter& iter,const Func& func)const{
		using State = std::remove_pointer_t<decltype(iter->state)>;
		State* state = iter->state;
		assert(
			state==nullptr || (!(state->is_blocked()) && state->is_connected())
		);
		if (state == nullptr) {
			return func(iter);
		}
	
		state->lock();
		auto when_exit = [state](auto* ) {
			state->unblock();
		};
		std::unique_ptr<State, decltype(when_exit)> ptr{state, when_exit};
		func(iter);
	}
};

template<>
struct lock_then_call<false> {
	template<class Func,class Iter>
	decltype(auto) operator()(const Iter& iter,const Func& func)const{
		using State = std::remove_pointer_t<decltype(iter->state)>;
		State* state = iter->state;
		assert(
			state==nullptr || (!(state->is_blocked()) && state->is_connected())
		);
		if (state == nullptr) {
			return func(iter);
		}
	
		state->lock();
		auto when_exit = [state](auto* ) {
			state->unblock();
		};
		auto&& res = func(iter);
		std::unique_ptr<State, decltype(when_exit)> ptr( state, when_exit );
		return forward_m(res);
	}
};
#endif
namespace Talg{

template <class Ref>
class RefWrapper {
	std::remove_reference_t<Ref>* ptr=nullptr;
public:
	static_assert(std::is_reference<Ref>::value, "RefWrapper should be used for reference only.");
	using type=Ref;
	
	RefWrapper() = default;
	RefWrapper(Ref ref) noexcept : ptr(std::addressof(ref)) {}
	RefWrapper(const RefWrapper&) noexcept = default;
	RefWrapper(RefWrapper&&) noexcept = default;
	
	RefWrapper& operator=(const RefWrapper& x) noexcept = default;
	RefWrapper& operator=(RefWrapper&& x) noexcept = default;
	RefWrapper& operator=(Ref x) noexcept {
		static_assert(std::is_reference<Ref>::value, "RefWrapper should be used for reference only.");
		ptr = std::addressof(x);
		return *this;
	}
	operator Ref () const noexcept { return forward_m(*ptr); }
	Ref get() const noexcept { return forward_m(*ptr); }
	void reset(Ref ref) {
		ptr = &ref;
	}
};
template<class T>
struct OptionalVal {
	union {
		T val;
		bool dummy;
	}val;
	bool is_active=false;
	OptionalVal()noexcept = default;
	void operator=(OptionalVal) = delete;
	OptionalVal(OptionalVal&&) = delete;

	template<class Iter,class...Ts>
	void reset(const Iter& iter,Ts&&...args) {
		if(is_active)
			val.val.~T();
		new(&(val.val)) T( (*iter)(std::forward<Ts>(args)...) );
		is_active = true;
	}
	
	explicit operator bool()const noexcept {
		return is_active;
	}
	void reset()noexcept {
		if (is_active) {
			val.val.~T();
			is_active = false;
		}
	}
	const T& get()const noexcept{
		return val.val;
	}
	T& get()noexcept{
		return val.val;
	}
	~OptionalVal() {
		reset();
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
	bool is_called=false;
	template<class Iter,class...Ts>
	void reset(const Iter&iter,Ts&&...args) { 
		(*iter)(std::forward<Ts>(args)...); 
		is_called = true;
	}
	void reset()noexcept{
		is_called = false;
	}
	explicit operator bool()const noexcept {
		return is_called;
	}
	const CacheRes<void>& get()const noexcept{
		return *this;
	}
};
template<class R>
struct CacheRes<R&&>{
private:
	RefWrapper<R&&> ref;
	bool is_called=false;
public:
	using reference_type	= R&&;
	using value_type		= R&&;
	using pointer			= R*;
	template<class Iter,class...Ts>
	void reset(const Iter&iter,Ts&&...args) { 
		ref=(*iter)(std::forward<Ts>(args)...); 
		is_called = true;
	}
	void reset()noexcept{
		is_called = false;
	}
	explicit operator bool()const noexcept {
		return is_called;
	}
	reference_type get()const noexcept{
		return ref.get();
	}
	reference_type get()noexcept{
		return ref.get();
	}
};


}//namespace Talg








#if 0
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
			[ptr,&args...](const Iter&iter) {
				new(ptr) T( *iter(std::forward<Ts>(args)...) );
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

#endif

#include "undef_macro.h"
