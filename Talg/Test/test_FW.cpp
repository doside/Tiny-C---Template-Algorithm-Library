#include <Talg/function_wrapper.h>
#include <Talg/slotlist.h>
#include <string>
#include "test_suits.h"
namespace {
	using namespace Talg;
	using UserSlotTraits = DefaultSlotTraits<std::function<void(int)>>;


	//此处是随便写的,因为会通过rebind机制而变化为正确的类型
	//(在下面的例子中正确的类型为void(int))
	template<class T=void(double, double)>	
	using UserSlotTraits2=DefaultSlotTraits<std::function<T>>;
	struct Fobj {
		void operator()(int)const{}
		template<class T>
		Fobj(T){}
	};
	void f() {
		
		SimpleSignal<void(int), DefaultSlotTraits<Fobj>> sig0;
		sig0 += [] {};
		sig0(1);

		SimpleSignal<void(int), UserSlotTraits> sig;
		sig += [] {};
		sig(1);
		SimpleSignal<void(int), UserSlotTraits2<>> sig2;
		sig2 += [] {};
		sig2(1);
		testSame(
			typename decltype(sig2)::SlotTrait, 
			UserSlotTraits2<void(int)>
		);
		//todo: support such usage sig -= nullptr;
		testSame(
			MapParamId<void(double,char),void(double, char, const std::string&)>,
			IdSeq<0,1>,
			ReplaceParam<Seq<int>,double>,
			Seq<double>
			//DefaultSlotTraits<EqualableFunction<void(int)>>,
			//ReplaceParam<UserSlotTraits,void(int)>
		);
	}
}