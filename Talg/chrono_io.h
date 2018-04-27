#pragma once
#include <chrono>
#include <iosfwd>
#include <Talg/core.h>
#include <Talg/basic_macro_impl.h>

namespace Talg{
	using namespace std::chrono_literals;

	
	/*!
		\brief	used to format std::duration.
		\tparam	T the ratio type of the duration.
	*/
	template<class T>
	struct TimePeriodTag{
		static constexpr const char* name() {		 
			return "s with ratio";
		}	
	};

#define DefTimePeriodTag_macro(type,txt)			 \
	template<>										 \
	struct TimePeriodTag<type> {					 \
		static constexpr const char* name() {		 \
			return txt;								 \
		}											 \
	}								 

	DefTimePeriodTag_macro(std::nano, "ns");
	DefTimePeriodTag_macro(std::micro, "us");
	DefTimePeriodTag_macro(std::milli, "ms");
	DefTimePeriodTag_macro(std::ratio<1>, "s");
	DefTimePeriodTag_macro(std::ratio<60>, "min");
	DefTimePeriodTag_macro(std::ratio<3600>, "h");

#undef DefTimePeriodTag_macro

	
	/*!
		\brief	print the duration to the output stream.
		\param	os the stream.	x the
		\return the stream
		\note	use the abbreviated noun,such as 'ms' 'ns' etc.
	*/
	template <class Out,class Rep,class Period>
	auto& operator<<(Out& out,const std::chrono::duration<Rep, Period>& x) 
	{
		using namespace Talg;
		return out<<x.count()<<TimePeriodTag<Period>::name();
	}
}

#include <Talg/undef_macro.h>
