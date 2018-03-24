#pragma once
#include <memory>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <Talg/tag_type.h>
#include <Talg/const_if.h>
#include <Talg/basic_macro_impl.h>

namespace Talg{
	/*!
		\brief	RAII惯用法 资源守卫
		\note	使用了deduce_t作为参数后可以直接通过makeit<Guard<>>([]{...});
				构造出一个guard而无需显式写出F
	*/
	template<class Func,class Tag=trival_case_t>
	class Guard:protected std::decay_t<Func> {
	public:
		using Base=Func;
		bool dismiss = false;
	public:
		Base& base()noexcept { return *this; }

		template<class T>
		Guard(T&& f,trival_case_t={})noexcept(std::is_nothrow_constructible<Base,T&&>::value)
			:Base(forward_m(f)){
			static_assert(noexcept(f()),
				"f is called within Guard::~Guard, and f is not noexcept but "
				"the destructor shall be noexcept; So "
				"you should use other tag to init Guard or use a noexcept functor init Guard."
				);
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
	
	template<class F,class Tag=trival_case_t>
	auto makeGuard(F&& func, Tag={}) {
		return Guard<F, Tag>(func,Tag{});
	}
}
#include <Talg/undef_macro.h>
