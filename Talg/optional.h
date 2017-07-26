#pragma once

// Copyright (C) 2011 - 2012 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal


// Modified by Landersing 2017.7.25: 
// add noexcept as more as possible,and change namespace to Talg.

# ifndef OPTIONAL_INCLUDED_H_
# define OPTIONAL_INCLUDED_H_

#include <utility>
#include <type_traits>
#include <initializer_list>
#include <cassert>
#include <functional>
#include <string>
#include <stdexcept>
#include "type_traits.h"


namespace Talg{

	// 20.5.4, optional for object types
	template <class T> class optional;

	// 20.5.5, optional for lvalue reference types
	template <class T> class optional<T&>;

	namespace detail
	{
		using std::enable_if_t;
		// static_addressof: a constexpr version of addressof
		template <class T>
		struct has_overloaded_addressof
		{
		  template <class X>
		  constexpr static bool has_overload(...) { return false; }
  
		  template <class X, size_t S = sizeof(std::declval<X&>().operator&()) >
		  constexpr static bool has_overload(bool) { return true; }

		  constexpr static bool value = has_overload<T>(true);
		};

		template <class T,class = enable_if_t<!has_overloaded_addressof<T>::value>>
		constexpr T* static_addressof(T& ref)
		{
		  return &ref;
		}

		template <class T>
		T* static_addressof(T& ref,enable_if_t<has_overloaded_addressof<T>::value,int> = 0)
		{
		  return std::addressof(ref);
		}


		// the call to convert<A>(b) has return type A and converts b to type A iff b decltype(b) is implicitly convertible to A  
		template <class U>
		/*constexpr*/ U convert(U v) { return v; }
  
	} // namespace detail

	

	constexpr struct trivial_init_t{} trivial_init{};


	// 20.5.6, In-place construction
	constexpr struct in_place_t{} in_place{};


	// 20.5.7, Disengaged state indicator
	struct nullopt_t
	{
	  struct init{};
	  constexpr explicit nullopt_t(init)noexcept{}
	};
	constexpr nullopt_t nullopt{nullopt_t::init()};


	// 20.5.8, class bad_optional_access
	class bad_optional_access : public std::logic_error {
	public:
	  explicit bad_optional_access(const std::string& what_arg) : std::logic_error{what_arg} {}
	  explicit bad_optional_access(const char* what_arg) : std::logic_error{what_arg} {}
	};


	template <class T>
	union storage_t
	{
	  unsigned char dummy_;
	  T value_;

	  constexpr storage_t( trivial_init_t ) noexcept : dummy_(0) {};

	  template <class... Args>
	  constexpr storage_t( Args&&... args )
		  noexcept(std::is_nothrow_constructible<T,Args&&...>::value)
		  : value_(std::forward<Args>(args)...) {}

	  ~storage_t(){}
	};


	template <class T>
	union constexpr_storage_t
	{
		unsigned char dummy_;
		T value_;

		constexpr constexpr_storage_t( trivial_init_t ) noexcept : dummy_() {};

		template <class... Args>
		constexpr constexpr_storage_t( Args&&... args ) 
			noexcept(std::is_nothrow_constructible<T,Args&&...>::value)
			: value_(std::forward<Args>(args)...) {}

		~constexpr_storage_t() = default;
	};


	template <class T>
	struct optional_base
	{
		bool init_;
		storage_t<T> storage_;

		constexpr optional_base() noexcept : init_(false), storage_(trivial_init) {};

		explicit constexpr optional_base(const T& v) : init_(true), storage_(v) {}

		explicit constexpr optional_base(T&& v) : init_(true), storage_(std::move(v)) {}

		template <class... Args> explicit optional_base(in_place_t, Args&&... args)
			noexcept(std::is_nothrow_constructible<storage_t<T>,Args&&...>::value)
			: init_(true), storage_(std::forward<Args>(args)...) {}


		template <class U, class... Args, 
			class=std::enable_if_t<
				std::is_constructible<T, std::initializer_list<U> >::value
			>
		>
		explicit optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
			noexcept(std::is_nothrow_constructible<storage_t<T>,std::initializer_list<U>,Args&&...>::value)
			: init_(true), storage_(il, std::forward<Args>(args)...) {}



		~optional_base() { if (init_) storage_.value_.T::~T(); }
	};


	template <class T>
	struct constexpr_optional_base
	{
		bool init_;
		constexpr_storage_t<T> storage_;

		constexpr constexpr_optional_base() noexcept : init_(false), storage_(trivial_init) {};

		explicit constexpr constexpr_optional_base(const T& v) : init_(true), storage_(v) {}

		explicit constexpr constexpr_optional_base(T&& v) 
			noexcept(std::is_nothrow_constructible<constexpr_storage_t<T>,T&&>::value)
			: init_(true), storage_(std::move(v)) {}

		template <class... Args> 
		explicit constexpr constexpr_optional_base(in_place_t, Args&&... args)
			noexcept(std::is_nothrow_constructible<constexpr_storage_t<T>,Args&&...>::value)
			: init_(true), storage_(std::forward<Args>(args)...) {}


		template <class U, class... Args, 
			class = std::enable_if_t<
				std::is_constructible<T, std::initializer_list<U>>::value
			>
		>
		explicit constexpr_optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
			noexcept(std::is_nothrow_constructible<constexpr_storage_t<T>,std::initializer_list<U>,Args&&...>::value)
			: init_(true), storage_(il, std::forward<Args>(args)...) {}

		~constexpr_optional_base() = default;
	};

	template <class T>
	using OptionalBase = std::conditional_t<
		std::is_trivially_destructible<T>::value,        // if possible
		constexpr_optional_base<std::remove_const_t<T>>, // use base with trivial destructor
		optional_base<std::remove_const_t<T>>
	>;

	template <class T>
	class optional : private OptionalBase<T>
	{
		using Base=OptionalBase<T>;
		static_assert( !std::is_same<std::decay_t<T>, nullopt_t>::value, "bad T" );
		static_assert( !std::is_same<std::decay_t<T>, in_place_t>::value, "bad T" );
		

		constexpr bool initialized() const noexcept { return Base::init_; }
		std::remove_const_t<T>* dataptr() {  return std::addressof(Base::storage_.value_); }
		constexpr const T* dataptr() const { return detail::static_addressof(Base::storage_.value_); }
  

		constexpr const T& contained_val() const& { return Base::storage_.value_; }
		T&& contained_val() && { return std::move(Base::storage_.value_); }
		T& contained_val() & { return Base::storage_.value_; }


		void clear() noexcept {
			if (initialized()) dataptr()->T::~T();
			Base::init_ = false;
		}
  
		template <class... Args>
		void initialize(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
		{
			assert(!Base::init_);
			::new (static_cast<void*>(dataptr())) T(std::forward<Args>(args)...);
			Base::init_ = true;
		}

		template <class U, class... Args>
		void initialize(std::initializer_list<U> il, Args&&... args) noexcept(noexcept(T(il, std::forward<Args>(args)...)))
		{
			assert(!Base::init_);
			::new (static_cast<void*>(dataptr())) T(il, std::forward<Args>(args)...);
			Base::init_ = true;
		}

	public:
		using value_type = T;

		// 20.5.5.1, constructors
		constexpr optional() noexcept : Base()  {};
		constexpr optional(nullopt_t) noexcept : Base() {};

		optional(const optional& rhs)
			noexcept(std::is_nothrow_copy_constructible<T>::value)
			: Base()
		{
			if (rhs.initialized()) {
				::new (static_cast<void*>(dataptr())) T(*rhs);
				Base::init_ = true;
			}
		}

		optional(optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
			: Base()
		{
			if (rhs.initialized()) {
				::new (static_cast<void*>(dataptr())) T(std::move(*rhs));
				Base::init_ = true;
			}
		}

		constexpr optional(const T& v)
			noexcept(std::is_nothrow_constructible<Base,const T&>::value)
			: Base(v) {}

		constexpr optional(T&& v)
			noexcept(std::is_nothrow_constructible<Base,T&&>::value)
			: Base(std::move(v)) {}

		template <class... Args>
		explicit constexpr optional(in_place_t, Args&&... args)
			noexcept(std::is_nothrow_constructible<Base,Args&&...>::value)
			: Base(in_place_t{}, std::forward<Args>(args)...) {}


		template <class U, class... Args, 
			class= std::enable_if_t<
				std::is_constructible<T, std::initializer_list<U>,Args&&...>::value
			>
		>
		explicit optional(in_place_t, std::initializer_list<U> il, Args&&... args)
			noexcept(std::is_nothrow_constructible<Base>::value)
			: Base(in_place_t{}, il, std::forward<Args>(args)...) {}


		// 20.5.4.2, Destructor
		~optional() = default;

		// 20.5.4.3, assignment
		optional& operator=(nullopt_t) noexcept
		{
			clear();
			return *this;
		}
		optional& operator=(const optional& rhs)
		  noexcept(
			  noexcept(std::declval<optional&>().initialize(*rhs))&&
			  noexcept(std::declval<optional&>().contained_val() = *rhs)
		  )
		{
			if (&rhs == this)
				return *this;
			if      (initialized() == true  && rhs.initialized() == false) clear();
			else if (initialized() == false && rhs.initialized() == true)  initialize(*rhs);
			else if (initialized() == true  && rhs.initialized() == true)  contained_val() = *rhs;
			return *this;
		}
  
		optional& operator=(optional&& rhs)
			noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value)
		{
			if (&rhs == this)
				return *this;
			if      (initialized() == true  && rhs.initialized() == false) clear();
			else if (initialized() == false && rhs.initialized() == true)  initialize(std::move(*rhs));
			else if (initialized() == true  && rhs.initialized() == true)  contained_val() = std::move(*rhs);
			return *this;
		}

		template <class U>
		auto operator=(U&& v)
			noexcept(
				noexcept(std::declval<optional&>().initialize(std::forward<U>(v))) &&
				noexcept(std::declval<optional&>().contained_val() = std::forward<U>(v))
			)
		-> std::enable_if_t<
				std::is_same<std::decay_t<U>, T>::value,
				optional&
			>
		{
			if (initialized()) { contained_val() = std::forward<U>(v); }
			else               { initialize(std::forward<U>(v));  }
			return *this;
		}
  
  
		template <class... Args>
		void emplace(Args&&... args)
			noexcept(
				noexcept(std::declval<optional&>().initialize(std::forward<Args>(args)...)) 
			)
		{
			clear();
			initialize(std::forward<Args>(args)...);
		}
  
		template <class U, class... Args>
		void emplace(std::initializer_list<U> il, Args&&... args)
			noexcept(
				noexcept(std::declval<optional&>().initialize(il,std::forward<Args>(args)...)) 
			)
		{
			clear();
			initialize<U, Args...>(il, std::forward<Args>(args)...);
		}
  
		// 20.5.4.4, Swap
		void swap(optional<T>& rhs) 
			noexcept(
				std::is_nothrow_move_constructible<T>::value && 
				noexcept(Talg::is_nothrow_swapable<T>::value)
			)
		{
			if      (initialized() == true  && rhs.initialized() == false) { rhs.initialize(std::move(**this)); clear(); }
			else if (initialized() == false && rhs.initialized() == true)  { initialize(std::move(*rhs)); rhs.clear(); }
			else if (initialized() == true  && rhs.initialized() == true)  { using std::swap; swap(**this, *rhs); }
		}

		// 20.5.4.5, Observers
  
		explicit constexpr operator bool() const noexcept { return initialized(); }

		constexpr bool has_value() const noexcept { return initialized(); }
  
		constexpr T const* operator ->() const {
			//assert(initialized());
			return dataptr();
		}
 

		T* operator ->() {
			assert(initialized());
			return dataptr();
		}
  
		constexpr T const& operator *() const {
			
			return initialized()?contained_val():([&]{assert(initialized());}(),contained_val());
		}
  
		T& operator *() {
			assert(initialized());
			return contained_val();
		}
  
		constexpr T const& value() const {
			return initialized() ? contained_val() : (throw bad_optional_access("bad optional access"), contained_val());
		}
  
		T& value() {
			return initialized() ? contained_val() : (throw bad_optional_access("bad optional access"), contained_val());
		}
  
  

		template <class V>
		constexpr T value_or(V&& v) const& {
			return *this ? **this : detail::convert<T>(std::forward<V>(v));
		}
 
		template <class V>
		T value_or(V&& v) && {
			return *this ? std::move(const_cast<optional<T>&>(*this).contained_val()) 
				: detail::convert<T>(std::forward<V>(v));
		}

		// 20.6.3.6, modifiers
		void reset() noexcept { 
			clear(); 
		}
	};

#if 0
	template <class T>
	class optional<T&>
	{
	  static_assert( !std::is_same<T, nullopt_t>::value, "bad T" );
	  static_assert( !std::is_same<T, in_place_t>::value, "bad T" );
	  T* ref;
  
	public:

	  // 20.5.5.1, construction/destruction
	  constexpr optional() noexcept : ref(nullptr) {}
  
	  constexpr optional(nullopt_t) noexcept : ref(nullptr) {}
   
	  constexpr optional(T& v) noexcept : ref(detail::static_addressof(v)) {}
  
	  optional(T&&) = delete;
  
	  constexpr optional(const optional& rhs) noexcept : ref(rhs.ref) {}
  
	  explicit constexpr optional(in_place_t, T& v) noexcept : ref(detail::static_addressof(v)) {}
  
	  explicit optional(in_place_t, T&&) = delete;
  
	  ~optional() = default;
  
	  // 20.5.5.2, mutation
	  optional& operator=(nullopt_t) noexcept {
		ref = nullptr;
		return *this;
	  }
  
	  // optional& operator=(const optional& rhs) noexcept {
		// ref = rhs.ref;
		// return *this;
	  // }
  
	  // optional& operator=(optional&& rhs) noexcept {
		// ref = rhs.ref;
		// return *this;
	  // }
  
	  template <class U>
	  auto operator=(U&& rhs) noexcept
	  -> typename enable_if
	  <
		is_same<typename decay<U>::type, optional<T&>>::value,
		optional&
	  >::type
	  {
		ref = rhs.ref;
		return *this;
	  }
  
	  template <class U>
	  auto operator=(U&& rhs) noexcept
	  -> typename enable_if
	  <
		!is_same<typename decay<U>::type, optional<T&>>::value,
		optional&
	  >::type
	  = delete;
  
	  void emplace(T& v) noexcept {
		ref = detail::static_addressof(v);
	  }
  
	  void emplace(T&&) = delete;
  
  
	  void swap(optional<T&>& rhs) noexcept
	  {
		std::swap(ref, rhs.ref);
	  }
    
	  // 20.5.5.3, observers
	  constexpr T* operator->() const {
		return TR2_OPTIONAL_ASSERTED_EXPRESSION(ref, ref);
	  }
  
	  constexpr T& operator*() const {
		return TR2_OPTIONAL_ASSERTED_EXPRESSION(ref, *ref);
	  }
  
	  constexpr T& value() const {
		return ref ? *ref : (throw bad_optional_access("bad optional access"), *ref);
	  }
  
	  explicit constexpr operator bool() const noexcept {
		return ref != nullptr;
	  }
 
	  constexpr bool has_value() const noexcept {
		return ref != nullptr;
	  }
  
	  template <class V>
	  constexpr typename decay<T>::type value_or(V&& v) const
	  {
		return *this ? **this : detail::convert<typename decay<T>::type>(constexpr_forward<V>(v));
	  }

	  // x.x.x.x, modifiers
	  void reset() noexcept { ref = nullptr; }
	};
#endif

	template <class T>
	class optional<T&>
	{
	  static_assert( sizeof(T) == 0, "optional references disallowed" );
	};


	// 20.5.8, Relational operators
	template <class T> constexpr bool operator==(const optional<T>& x, const optional<T>& y)
	{
	  return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
	}

	template <class T> constexpr bool operator!=(const optional<T>& x, const optional<T>& y)
	{
	  return !(x == y);
	}

	template <class T> constexpr bool operator<(const optional<T>& x, const optional<T>& y)
	{
	  return (!y) ? false : (!x) ? true : *x < *y;
	}

	template <class T> constexpr bool operator>(const optional<T>& x, const optional<T>& y)
	{
	  return (y < x);
	}

	template <class T> constexpr bool operator<=(const optional<T>& x, const optional<T>& y)
	{
	  return !(y < x);
	}

	template <class T> constexpr bool operator>=(const optional<T>& x, const optional<T>& y)
	{
	  return !(x < y);
	}


	// 20.5.9, Comparison with nullopt
	template <class T> constexpr bool operator==(const optional<T>& x, nullopt_t) noexcept
	{
	  return (!x);
	}

	template <class T> constexpr bool operator==(nullopt_t, const optional<T>& x) noexcept
	{
	  return (!x);
	}

	template <class T> constexpr bool operator!=(const optional<T>& x, nullopt_t) noexcept
	{
	  return bool(x);
	}

	template <class T> constexpr bool operator!=(nullopt_t, const optional<T>& x) noexcept
	{
	  return bool(x);
	}

	template <class T> constexpr bool operator<(const optional<T>&, nullopt_t) noexcept
	{
	  return false;
	}

	template <class T> constexpr bool operator<(nullopt_t, const optional<T>& x) noexcept
	{
	  return bool(x);
	}

	template <class T> constexpr bool operator<=(const optional<T>& x, nullopt_t) noexcept
	{
	  return (!x);
	}

	template <class T> constexpr bool operator<=(nullopt_t, const optional<T>&) noexcept
	{
	  return true;
	}

	template <class T> constexpr bool operator>(const optional<T>& x, nullopt_t) noexcept
	{
	  return bool(x);
	}

	template <class T> constexpr bool operator>(nullopt_t, const optional<T>&) noexcept
	{
	  return false;
	}

	template <class T> constexpr bool operator>=(const optional<T>&, nullopt_t) noexcept
	{
	  return true;
	}

	template <class T> constexpr bool operator>=(nullopt_t, const optional<T>& x) noexcept
	{
	  return (!x);
	}



	// 20.5.10, Comparison with T
	template <class T> constexpr bool operator==(const optional<T>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x == v : false))
	{
	  return bool(x) ? *x == v : false;
	}

	template <class T> constexpr bool operator==(const T& v, const optional<T>& x)
		noexcept(noexcept(bool(x) ? v == *x : false))
	{
	  return bool(x) ? v == *x : false;
	}

	template <class T> constexpr bool operator!=(const optional<T>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x != v : true))
	{
	  return bool(x) ? *x != v : true;
	}

	template <class T> constexpr bool operator!=(const T& v, const optional<T>& x)
		noexcept(noexcept(bool(x) ? v != *x : true))
	{
	  return bool(x) ? v != *x : true;
	}

	template <class T> constexpr bool operator<(const optional<T>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x < v : true))
	{
	  return bool(x) ? *x < v : true;
	}

	template <class T> constexpr bool operator>(const T& v, const optional<T>& x)
		noexcept(noexcept(bool(x) ? v > *x : true))
	{
	  return bool(x) ? v > *x : true;
	}

	template <class T> constexpr bool operator>(const optional<T>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x > v : false))
	{
	  return bool(x) ? *x > v : false;
	}

	template <class T> constexpr bool operator<(const T& v, const optional<T>& x)
		noexcept(noexcept(bool(x) ? v < *x : false))
	{
	  return bool(x) ? v < *x : false;
	}

	template <class T> constexpr bool operator>=(const optional<T>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x >= v : false))
	{
	  return bool(x) ? *x >= v : false;
	}

	template <class T> constexpr bool operator<=(const T& v, const optional<T>& x)
		noexcept(noexcept(bool(x) ? v <= *x : false))
	{
	  return bool(x) ? v <= *x : false;
	}

	template <class T> constexpr bool operator<=(const optional<T>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x <= v : true))
	{
	  return bool(x) ? *x <= v : true;
	}

	template <class T> constexpr bool operator>=(const T& v, const optional<T>& x)
		noexcept(noexcept(bool(x) ? v >= *x : true))
	{
	  return bool(x) ? v >= *x : true;
	}


	// Comparison of optional<T&> with T
	template <class T> constexpr bool operator==(const optional<T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x == v : false))
	{
	  return bool(x) ? *x == v : false;
	}

	template <class T> constexpr bool operator==(const T& v, const optional<T&>& x)
		noexcept(noexcept(bool(x) ? v == *x : false))
	{
	  return bool(x) ? v == *x : false;
	}

	template <class T> constexpr bool operator!=(const optional<T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x != v : true))
	{
	  return bool(x) ? *x != v : true;
	}

	template <class T> constexpr bool operator!=(const T& v, const optional<T&>& x)
		noexcept(noexcept(bool(x) ? v != *x : true))
	{
	  return bool(x) ? v != *x : true;
	}

	template <class T> constexpr bool operator<(const optional<T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x < v : true))
	{
	  return bool(x) ? *x < v : true;
	}

	template <class T> constexpr bool operator>(const T& v, const optional<T&>& x)
		noexcept(noexcept(bool(x) ? v > *x : true))
	{
	  return bool(x) ? v > *x : true;
	}

	template <class T> constexpr bool operator>(const optional<T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x > v : false))
	{
	  return bool(x) ? *x > v : false;
	}

	template <class T> constexpr bool operator<(const T& v, const optional<T&>& x)
		noexcept(noexcept(bool(x) ? v < *x : false))
	{
	  return bool(x) ? v < *x : false;
	}

	template <class T> constexpr bool operator>=(const optional<T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x >= v : false))
	{
	  return bool(x) ? *x >= v : false;
	}

	template <class T> constexpr bool operator<=(const T& v, const optional<T&>& x)
		noexcept(noexcept(bool(x) ? v <= *x : false))
	{
	  return bool(x) ? v <= *x : false;
	}

	template <class T> constexpr bool operator<=(const optional<T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x <= v : true))
	{
	  return bool(x) ? *x <= v : true;
	}

	template <class T> constexpr bool operator>=(const T& v, const optional<T&>& x)
		noexcept(noexcept(bool(x) ? v >= *x : true))
	{
	  return bool(x) ? v >= *x : true;
	}

#if 0
	// Comparison of optional<T const&> with T
	template <class T> constexpr bool operator==(const optional<const T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x == v : false))
	{
	  return bool(x) ? *x == v : false;
	}

	template <class T> constexpr bool operator==(const T& v, const optional<const T&>& x)
		noexcept(noexcept(bool(x) ? v == *x : false))
	{
	  return bool(x) ? v == *x : false;
	}

	template <class T> constexpr bool operator!=(const optional<const T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x != v : true))
	{
	  return bool(x) ? *x != v : true;
	}

	template <class T> constexpr bool operator!=(const T& v, const optional<const T&>& x)
		noexcept(noexcept(bool(x) ? v != *x : true))
	{
	  return bool(x) ? v != *x : true;
	}

	template <class T> constexpr bool operator<(const optional<const T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x < v : true))
	{
	  return bool(x) ? *x < v : true;
	}

	template <class T> constexpr bool operator>(const T& v, const optional<const T&>& x)
		noexcept(noexcept(bool(x) ? v > *x : true))
	{
	  return bool(x) ? v > *x : true;
	}

	template <class T> constexpr bool operator>(const optional<const T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x > v : false))
	{
	  return bool(x) ? *x > v : false;
	}

	template <class T> constexpr bool operator<(const T& v, const optional<const T&>& x)
		noexcept(noexcept(bool(x) ? v < *x : false))
	{
	  return bool(x) ? v < *x : false;
	}

	template <class T> constexpr bool operator>=(const optional<const T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x >= v : false))
	{
	  return bool(x) ? *x >= v : false;
	}

	template <class T> constexpr bool operator<=(const T& v, const optional<const T&>& x)
		noexcept(noexcept(bool(x) ? v <= *x : false))
	{
	  return bool(x) ? v <= *x : false;
	}

	template <class T> constexpr bool operator<=(const optional<const T&>& x, const T& v)
		noexcept(noexcept(bool(x) ? *x <= v : tru))
	{
	  return bool(x) ? *x <= v : true;
	}

	template <class T> constexpr bool operator>=(const T& v, const optional<const T&>& x)
		noexcept(noexcept(bool(x) ? v >= *x : true))
	{
	  return bool(x) ? v >= *x : true;
	}
#endif


	// 20.5.12, Specialized algorithms
	template <class T>
	void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
	{
	  x.swap(y);
	}


	template <class T>
	constexpr optional<std::decay_t<T>> make_optional(T&& v)
	{
	  return optional<std::decay_t<T>>(std::forward<T>(v));
	}

	template <class X>
	constexpr optional<X&> make_optional(std::reference_wrapper<X> v)
	{
	  return optional<X&>(v.get());
	}


}//namespace Talg

namespace std
{
  template <class T>
  struct hash<Talg::optional<T>>
  {
    typedef typename hash<T>::result_type result_type;
    typedef Talg::optional<T> argument_type;
    
    constexpr result_type operator()(argument_type const& arg) const {
      return arg ? std::hash<T>{}(*arg) : result_type{};
    }
  };
  
  template <class T>
  struct hash<Talg::optional<T&>>
  {
    typedef typename hash<T>::result_type result_type;
    typedef Talg::optional<T&> argument_type;
    
    constexpr result_type operator()(argument_type const& arg) const {
      return arg ? std::hash<T>{}(*arg) : result_type{};
    }
  };
}



# endif //OPTIONAL_INCLUDED_H_

