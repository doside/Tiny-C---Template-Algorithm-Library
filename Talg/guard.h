#pragma once
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <Talg/basic_macro_impl.h>

namespace Talg{
	template<class F>
	class Guard:public std::decay_t<F> {
	public:
		using Base=std::decay_t<F>;
		bool dimiss = false;
	public:
		Base& base()noexcept { return *this; }
		template<class T>
		Guard(T&& f)noexcept(std::is_nothrow_constructible<F,T&&>::value)
			:Base(forward_m(f)){}

		Guard(Guard&& rhs)noexcept(std::is_nothrow_move_constructible<F>::value)
			:Base(std::move(rhs.base())){ 
			dismiss = rhs.dimiss;
			rhs.dimiss = true;
		}

		Guard& operator=(Guard&& rhs) {
			if (&rhs != this) {
				base() = std::move(rhs.base());
				dimiss = rhs.dimiss;
				rhs.dimiss = true;
			}
			return *this;
		}

		~Guard() {
			//static_assert(noexcept(func()), "");
			if (dimiss)
				return;
			base()();
		}
	};
	


	template<class T,void(*dtor)(T*)>
	struct custom_deleter
	{
	   constexpr custom_deleter() noexcept = default;
	   void operator()(T* ptr) const
	   {
		  dtor(ptr);
	   }
	};

	template<class T,void(*dtor)(T*)>
	struct UniquePtr: std::unique_ptr<T,custom_deleter<T,dtor>>{
		using Base=std::unique_ptr<T,custom_deleter<T,dtor>>;
		template<class U>
		using rebind = UniquePtr<U, dtor>;


		template<class U,void(*other_del)(U*)>
		UniquePtr& operator=(UniquePtr<U,other_del>&& )=delete;
		
		template<class U>
		UniquePtr& operator=(U)=delete;
		
		template<class U,class E>
		UniquePtr(std::unique_ptr<U,E>)=delete;
		
		template<class U>
		UniquePtr(U )=delete;	
		

		constexpr UniquePtr()noexcept:Base() { }
		constexpr explicit UniquePtr(T* val): Base(val){}
		explicit UniquePtr(T* val, std::function<void()> error_check) {
			//Shall check the val then could assign it to base,or undefine behaviour will happen.
			if (val == nullptr) {
				error_check();
			}
			this->reset(val);
		}
		UniquePtr(Base&& rval)noexcept:Base(std::move(rval)) {}

		UniquePtr& operator=(UniquePtr&& ptr) {
			Base::operator=(std::move(ptr));
			return *this;
		}
	};

}
#include <Talg/undef_macro.h>
