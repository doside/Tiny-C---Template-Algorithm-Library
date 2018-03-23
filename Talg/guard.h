#pragma once
#include <memory>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <Talg/tag_type.h>
#include <Talg/select_type.h>
#include <Talg/basic_macro_impl.h>

namespace Talg{
	/*!
		\brief	RAII范式的资源守卫
		\note	使用了deduce_t作为参数后可以直接通过makeit<Guard<>>([]{...});
				构造出一个guard而无需显式写出F
	*/
	template<class...Fs>
	class Guard:public std::decay_t<Head_t<Fs...>> {
	public:
		using Base=std::decay_t<Head_t<Fs...>>;
		using Tag = Tail_t<Fs...>;
		bool dimiss = false;
	public:
		Base& base()noexcept { return *this; }
		template<class T>
		Guard(T&& f)noexcept(std::is_nothrow_constructible<F,T&&>::value)
			:Base(forward_m(f)){
			static_assert(noexcept(f()), "析构函数中不可抛出异常");
		}
		
		template<class T>
		Guard(T&& f,weak_except_t)noexcept(std::is_nothrow_constructible<F,T&&>::value)
			:Base(forward_m(f)){ }
		/*!
			\brief
			\param
			\return
			\note	可能有严重效率损失.
		*/
		template<class T>
		Guard(T&& f,with_check_t)noexcept(std::is_nothrow_constructible<F,T&&>::value)
			:Base(forward_m(f)){ }

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

		static auto& reporter() {
			static std::function<void()> on_err([]{});
			return on_err;
		}
		template<class F>
		static auto& onError(F&& func) {
			static_assert(noexcept(std::forward<F>(func)()), "");
			return reporter() = func;
		}
		~Guard() {
			//防御性编程,同时尽可能不牺牲效率.
			
			if (dimiss)
				return;
			if (noexcept(base()())||std::is_same<Tag,weak_except_t>::value) {
				base()();
			} else {
				try {
					base()();
				}
				catch(...){
					reporter()();
				}
			}
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
