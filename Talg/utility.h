#pragma once

#include <stdexcept>	//runtime_error
#include "core.h"
#include "type_traits.h"
#include "basic_marco_impl.h"

namespace Talg {

	struct OptionalInit{};
	struct OptionalEmptyVisit:std::runtime_error{
		using std::runtime_error::runtime_error;
	};

	template<class T>
	struct Optional {
		static_assert(std::is_reference<T>::value == false, "");
		bool has_value;
		union {
			bool dummy_;
			T value;
		};
		
		constexpr Optional(OptionalInit&&)noexcept(std::is_nothrow_default_constructible<T>::value)
			:has_value(true),dummy_(true){}
		
		constexpr Optional()noexcept:has_value(false),dummy_(true) {}
		
		template<class...Ts>
		constexpr Optional(ExcludeBase<Optional,Ts>&&...args)
			//noexcept(noexcept(std::declval<T>(forward_m(args)...)))
			:has_value(true),value(forward_m(args)...){}
		
		Optional(const Optional& rhs) 
		:has_value(rhs.has_value),dummy_(true){
			*this = rhs;
		}
		
		Optional(Optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
		:has_value(false),dummy_(false){
			*this = std::move(rhs);
		}

		void destroy()noexcept {
			if (has_value) {
				value.~T();
			}
		}
		constexpr explicit operator bool()const noexcept {
			return has_value;
		}
		
		constexpr bool empty()const noexcept {
			return !has_value;
		}
		
		Optional& operator=(const Optional& rhs) {
			if (&rhs == this || rhs.has_value == false) {
				return *this;
			}
			return *this = rhs.value;
		}
		
		Optional& operator=(Optional&& rhs) {
			if (&rhs == this || rhs.has_value == false) {
				return *this;
			}
			rhs.has_value = false;
			return *this = std::move(rhs.value);
		}

		Optional& operator=(const T& rhs)
			noexcept(std::is_nothrow_copy_constructible<T>::value) {
			if (!has_value) {
				new (&value) T(rhs);
				has_value = true;
			} else {
				value = rhs;
			}
			return *this;
		}
		
		Optional& operator=(T&& rhs)noexcept(std::is_nothrow_move_assignable<T>::value) {
			if (!has_value) {
				new (&value) T(std::move(rhs));
				has_value = true;
			} else {
				has_value = false;
				value = rhs;
				has_value = true;
			}
			return *this;
		}
		
		template<class U>
		Optional& operator=(Tself<Optional,U>&& val) {
			if (!has_value) {
				new (&value) T(forward_m(val));
				has_value = true;
			} else {
				has_value = false;
				value = forward_m(val);
				has_value = true;
			}
			return *this;
		}

		T& operator*() {
			if (has_value) {
				return value;
			} else {
				throw OptionalEmptyVisit("dereference empty optional.");
			}
		}
		const T& operator*()const{
			if (has_value) {
				return value;
			} else {
				throw OptionalEmptyVisit("dereference empty optional.");
			}
		}
		~Optional() {
			destroy();
		}
	};


	struct EmptyExpected :std::runtime_error {
		using runtime_error::runtime_error;
	};

	template<class T>
	struct Expected:Optional<T> {
		using Optional<T>::Optional;
		operator T&()& {
			if (!this->has_value) {
				throw EmptyExpected("no value");
			}
			return **this;
		}
		operator T&&()&& {
			if (!this->has_value) {
				throw EmptyExpected("no value");
			}
			return std::move(**this);
		}
	};

	template<class T>
	struct Expected<T&> {
		std::remove_reference_t<T>* value_=nullptr;
		bool has_val=false;

		Expected(T& ref):value_(&ref){ }
		Expected()noexcept = default;

		Expected(Expected&& rhs)noexcept:value_(rhs.value_), has_val(true) {
			rhs.has_val = false;
			rhs.value_ = nullptr;
		}
		Expected& operator=(Expected&& rhs)noexcept{
			if (this == &rhs)
				return *this;
			has_val = true;
			value_ = rhs.value_;
			rhs.has_val = false;
		}

		operator T&()& {
			if (!has_val) {
				throw EmptyExpected("no value");
			}
			return *value_;
		}
		operator T&&()&& {
			if (!has_val) {
				throw EmptyExpected("no value");
			}
			return std::move(*value_);
		}
		constexpr explicit operator bool()const noexcept {
			return has_val;
		}
	};




}
#include "undef_macro.h"
