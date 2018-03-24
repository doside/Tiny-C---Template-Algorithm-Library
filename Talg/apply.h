#pragma once
#include <type_traits>
#include <Talg/basic_macro_impl.h>
namespace Talg {

	template<bool res>
	struct ApplyWhen {
		/*!
			\brief	当条件满足时就调用函数,并且返回std::true_type,
					从而可以检查是否成功调用.
			\param \func 被调用的函数 \args... 参数
			\return std::true_type
			\note	不返回bool,是因为func未必总是constexpr.
					此处不像ConstIf,当条件失败时没有选择另一个函数进行调用,
					理由是参数可能不太一样.
			\example 
						if(ApplyWhen<cond>(func1,params...)){
							//do something...
						}else{
							func2(params...);
							//do other things.
						}
						等价于
						constexpr if(cond){
							forward_m(func1)( forward_m(params)...);
						}else{
							func2(params...);
						}
		*/
		template<class F,class...Ts>
		constexpr std::true_type operator()(F&& func,Ts&&...args)const
		except_when_m(forward_m(func)(forward_m(args)...))
		{
			return (void)(forward_m(func)(forward_m(args)...)),std::true_type{};
		}
	};

	template<>
	struct ApplyWhen<false> {
		template<class...Ts>
		constexpr std::false_type operator()(Ts&&...)const noexcept{
			return std::false_type{};
		}
	};

}
#include <Talg/undef_macro.h>
