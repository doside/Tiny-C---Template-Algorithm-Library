#pragma once
#include <functional>	//for std::reference_wrapper
#include <Talg/fold.h>
#include <Talg/basic_macro_impl.h>


namespace Talg{
	template<class Stream>
	struct Ostream:std::reference_wrapper<Stream> {
		using Base = std::reference_wrapper<Stream>;
		using Base::Base;
		using Base::get;
		using Base::operator Stream&;


		//todo 没有办法写出合法的noexcept.
		template<class T>
		Ostream& operator<<(T&& obj) 
		{
			using namespace Talg;
			get() << forward_m(obj);
			return *this;
		}
		template<class...Ts>
		Ostream& operator()(Ts&&...args) {
			return foldl(
				[](Ostream& out,auto&& val){
					using namespace Talg;
					return out.get() << forward_m(val);
				}, 
				*this, forward_m(args)...
			);
		}
	};
}

#include <Talg/undef_macro.h>


