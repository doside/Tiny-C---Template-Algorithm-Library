#pragma once
#include <iosfwd>
#include <typeinfo>
#include <type_traits>	//for std::decay_t
#include <initializer_list>
#include <tuple>
#include <Talg/core.h>
#include <Talg/invoke_impl.h>
#include <Talg/basic_macro_impl.h>
namespace Talg{
	class AnyPtr {
	private:
		
		struct Info{

#define IO_FUNC_macro(type,name)	virtual type& name(type& out,void*)const{return out;}
			IO_FUNC_macro(std::ostream						,out_c		)
			IO_FUNC_macro(std::ostringstream				,out_s		)
			IO_FUNC_macro(std::wostringstream				,out_sw		)
			IO_FUNC_macro(std::wostream						,out_w		)
			IO_FUNC_macro(std::basic_ostream<char16_t>		,out_c16	)
			IO_FUNC_macro(std::basic_ostream<char32_t>		,out_c32	)
			IO_FUNC_macro(std::istream						,in_c		)
			IO_FUNC_macro(std::istringstream				,in_s		)
			IO_FUNC_macro(std::wistream						,in_w		)
			IO_FUNC_macro(std::wistringstream				,in_sw		)
			IO_FUNC_macro(std::basic_istream<char16_t>		,in_c16		)
			IO_FUNC_macro(std::basic_istream<char32_t>		,in_c32		)
#undef IO_FUNC_macro
			//no virtual dtor.
			virtual const std::type_info& get_type()const{
				return typeid(Info);	//todo use a tag instead.
			};
			static auto* make() {
				static Info res;
				return &res;
			}
		};
		void* data_;
		Info* vtable_;

		template<class T,class Base=Info,class Name=void>
		struct DInfo:Base {
			//todo detect if T has operator<< or operator>>
#define DEF_O_FUNC_macro(type)	\
			type& name(type& out,void* ptr)override{	\
				return  out<<*static_cast<T*>(ptr);		\
			}
#define DEF_I_FUNC_macro(type)	\
			type& name(type& in,void* ptr)override{		\
				return  in>> *static_cast<T*>(ptr);		\
			}		
			DEF_O_FUNC_macro(std::ostream					)
			DEF_O_FUNC_macro(std::ostringstream				)
			DEF_O_FUNC_macro(std::wostringstream			)
			DEF_O_FUNC_macro(std::wostream					)
			DEF_O_FUNC_macro(std::basic_ostream<char16_t>	)
			DEF_O_FUNC_macro(std::basic_ostream<char32_t>	)
			DEF_I_FUNC_macro(std::istream					)
			DEF_I_FUNC_macro(std::istringstream				)
			DEF_I_FUNC_macro(std::wistream					)
			DEF_I_FUNC_macro(std::wistringstream			)
			DEF_I_FUNC_macro(std::basic_istream<char16_t>	)
			DEF_I_FUNC_macro(std::basic_istream<char32_t>	)
#undef DEF_I_FUNC_macro
#undef DEF_O_FUNC_macro
			const std::type_info& get_type()const override{
				return typeid(T);
			}
			static auto* make()noexcept{
				static DInfo res;
				return &res;
			}
		};
	public:
		template<class T>
		explicit AnyPtr(T&& v)noexcept
			:data_(&v),vtable_(DInfo<std::decay_t<T>>::make()){}


		/*!
			\brief	
			\param
			\return
			\note	参数使用Base*是为了能直接写(Base*)nullptr
					当前暂时不支持多个基类.
			\example
					//in some place someone keep the object life,
					//and create a AnyPtr passed to others.
					
					//A send d1 to C
					Derived d_;
					send(c,AnyPtr(d_,(Base*)0));

					//B send d2 to C
					Derived2 d_;
					send(c,AnyPtr(d_,(Base*)0));

					//in other place.
					//someone received a bunch of AnyPtr.
					std::vector<AnyPtr> datas_;
					
					//then C maybe do something about Base 
					//without care about what type he had.
					for(auto& elem:datas){
						if(auto ptr=v.get<Base>()){
							ptr->func();	//polymorphic behaviour.
						}
					}
		*/

		template<class Base,class T>
		explicit AnyPtr(T&& v,Base*)noexcept
			:data_(&v),vtable_(DInfo<std::decay_t<T>,DInfo<std::decay_t<Base>>>::make()){
			static_assert(staticCheck<std::is_base_of<Base, T>, Base, T>(),"");
		}

		AnyPtr()noexcept
			:data_(nullptr),vtable_(Info::make()){}

		friend std::ostream& operator<<(std::ostream& out, const AnyPtr& v) {
			if (v.vtable_->out_c == nullptr)
				return out;
			return v.vtable_->out_c(out, v.data_);
		}
		template<class T>
		T* get()noexcept{
			auto* ptr = dynamic_cast<DInfo<std::decay_t<T>>*>(vtable_);
			if (ptr != nullptr) {
				return static_cast<T*>(data_);
			}
			return nullptr;
		}
		bool has_value()const noexcept { return data_ == nullptr; }
		const type_info& type() const noexcept;
		void reset()noexcept { 
			data_ = nullptr; 
			vtable_ = Info::make(); 
		}
		constexpr bool operator==(const AnyPtr& rhs)const noexcept {
			return data_ == rhs.data_;
		}
		constexpr bool operator!=(const AnyPtr& rhs)const noexcept {
			return !(*this == rhs);
		}
		template<class Base,class F,class...Ts>
		bool applyAs(F&& func,Ts&&...args) {
			auto* tmp = get<Base>();
			if (tmp) {
				ct_invoke(forward_m(func), tmp,forward_m(args)...);
				return true;
			}
			return false;
		}
	};


}

#include <Talg/undef_macro.h>


