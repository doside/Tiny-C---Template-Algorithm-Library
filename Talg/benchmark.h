#pragma once
#include <Talg/call.h>

#include <Talg/basic_macro_impl.h>
namespace Talg {

	/*!
		\brief	用于benchmark的时间计数.
				used for count time of one function.
		\tparam Clock clock type,which should meets the requirements of Concept TrivialClock
	*/
	template<class Clock>
	struct CountTime {
		/*
			\brief	用于测量单个函数的迭代若干次的执行时间,
					并且可以在计时开始前进行初始化准备.
					measure the run time of one function which be called iter_cnt times.
			\param	\func:		被测函数 
								the function be called. 
					\iter_cnt:	调用次数 
								count of calls.
					\fixtures:	用于预处理工作的函数列 
								a list of functions that be called before time countting.
			\return	the duration of the clock.
		*/
		template<class Func,class...Fixture>
		auto operator()(Func&& func,size_t iter_cnt,Fixture&&...fixtures) {
			forEach(forward_m(fixtures)...);
			auto prev = Clock::now();
			forward_m(func)();
			auto time = Clock::now() - prev;
			return time;
		}
	};












}
#include <Talg/undef_macro.h>


