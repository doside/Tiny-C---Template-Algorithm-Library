#pragma once
#include <memory>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <Talg/tag_type.h>
#include <Talg/select_type.h>
#include <Talg/const_if.h>
#include <Talg/basic_macro_impl.h>

namespace Talg{
	/*!
		\brief	RAII惯用法 资源守卫
		\note	使用了deduce_t作为参数后可以直接通过makeit<Guard<>>([]{...});
				构造出一个guard而无需显式写出F
	*/
	template<class...Fs>
	class Guard:protected std::decay_t<Head_s<Seq<Fs...>>> {
	public:
		using Base=std::decay_t<Head_s<Seq<Fs...>>>;
		using Tag = Tail_s<Seq<Fs...>>;
		bool dismiss = false;
	public:
		Base& base()noexcept { return *this; }
		template<class T>
		Guard(T&& f)noexcept(std::is_nothrow_constructible<Base,T&&>::value)
			:Base(forward_m(f)){
			static_assert(noexcept(f()), "析构函数中不可抛出异常");
		}
		
		template<class T>
		Guard(T&& f,weak_except_t)noexcept(std::is_nothrow_constructible<Base,T&&>::value)
			:Base(forward_m(f)){ }
	 
		template<class T>
		Guard(T&& f,with_check_t)noexcept(std::is_nothrow_constructible<Base,T&&>::value)
			:Base(forward_m(f)){ }

		Guard(Guard&& rhs)noexcept(std::is_nothrow_move_constructible<Base>::value)
			:Base(std::move(rhs.base())){ 
			dismiss = rhs.dismiss;
			rhs.dismiss = true;
		}

		Guard& operator=(Guard&& rhs) {
			if (&rhs != this) {
				base() = std::move(rhs.base());
				dismiss = rhs.dismiss;
				rhs.dismiss = true;
			}
			return *this;
		}

		static auto& reporter() {
			static std::function<void()> on_err([]{});
			return on_err;
		}
		template<class Base>
		static auto& onError(Base&& func) {
			static_assert(noexcept(std::forward<Base>(func)()), "");
			return reporter() = func;
		}
		~Guard() {
			if (dismiss)
				return;
			constIf<noexcept(base()()) || std::is_same<Tag, weak_except_t>::value>(
				[this]{ 
					base()();
				},
				[this]{	
					try {
						base()();
					}
					catch(...){
						reporter()();
					}
				}
			);
		}
	};
	

}
#include <Talg/undef_macro.h>
