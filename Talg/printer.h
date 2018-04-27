#pragma once
#include <functional>	//for std::reference_wrapper
#include <Talg/core.h>
#include <Talg/const_if.h>
#include <Talg/call.h>
#include <Talg/fold.h>
#include <Talg/type_traits.h>
#include <Talg/basic_macro_impl.h>


namespace Talg{

	struct BaseFormater{};
	

	template<class Stream>
	class DefaultFormater:public BaseFormater{
		std::reference_wrapper<Stream> out_;
		//using Base = OstreamRefBase<Stream>;
	public:
		constexpr DefaultFormater(Stream& ref)noexcept:out_{ref}{}
		auto& get()noexcept{
			return out_.get();
		}
		auto& get()const noexcept{
			return out_.get();
		}
		template<class T,class =WhenNoBaseOf<BaseFormater,T>>
		DefaultFormater& operator()(T&& first) {
			static_assert(IsDecaySame<T, DefaultFormater>::value == false, "");
			get() << forward_m(first);
			return *this;
		}
		template<class T,class=WhenBaseOf<BaseFormater,T>>
		decltype(auto) operator()(T&& fmt) {
			static_assert(IsDecaySame<T, DefaultFormater>::value == false, "");
			return fmt(std::move(*this));
		}
		
	};


	template<class Formater>
	class AutoSpaceState:public Formater{
		const char* str_;
	public:
		using Base = Formater;
		AutoSpaceState(Formater&& f, const char* delimter)
		:Base(std::move(f)), str_(delimter) {

		}

		//using Formater::operator();

		template<class T,class U,class=WhenNoBaseOf<BaseFormater,T>>
		AutoSpaceState& operator()(T&& val,U&&){
			using namespace Talg;
			Base::operator()(forward_m(val))<<str_;
			return *this;
		}

		template<class T,class=WhenNoBaseOf<BaseFormater,T>>
		AutoSpaceState& operator()(T&& val){
			using namespace Talg;
			Base::operator()(forward_m(val));
			return *this;
		}


	};

	struct AutoSpace:BaseFormater{
		const char* delimiter_;
		constexpr AutoSpace(const char* delimiter=" ")noexcept
		 :delimiter_(delimiter){
			
		}
		template<class Formater>
		auto operator()(Formater&& fmter) {
			return AutoSpaceState<std::decay_t<Formater>>{forward_m(fmter),delimiter_};
		}
	};

	struct Cancelter:BaseFormater{
		template<class Formater,
			class=WhenNoDecaySame<Formater,Transform<DefaultFormater,Formater>>
		>
		auto operator()(Formater&& fmter) {
			return typename Formater::Base(forward_m(fmter));
		}
		template<class T>
		auto operator()(DefaultFormater<T>&& fmter) {
			return forward_m(fmter);
		}
	};
	
	//
	//template<class T>
	//auto& operator<<(Adj<Out, Ts...>& out, T&& val) {
	//	forEach(
	//		[&val](auto& out_) {
	//			out_ << std::forward<T>(val);	//can not use forward_m in lambda.
	//		}, 
	//		static_cast<Ts&>(out)...
	//	);
	//	return out;
	//}

	

	template<class T,class...Ts>
	auto print(T&& stream,Ts&&...args) {
		auto f=[](auto&& out,auto&&...vs){
			using namespace Talg;
			return out(forward_m(vs)...);
		};
		return foldl(f, DefaultFormater<std::decay_t<T>>(stream),forward_m(args)...);
	}
	
	//template<class T>
	//auto to_string(T&& val) {
	//	using namespace Talg;
	//	using namespace std;
	//	return to_string(forward_m(val));
	//}

}



#include <Talg/undef_macro.h>


