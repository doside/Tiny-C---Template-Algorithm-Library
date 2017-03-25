#include "function_wrapper.h"
#include <string>
#include "test_suits.h"
#include "slotlist.h"
namespace {
	using namespace Talg;
	template<class T=void()>
	using UserSlotTraits = DefaultSlotTraits<T, std::function<T>>;
	void f() {
		SimpleSignal<void(int), UserSlotTraits<>> sig;
		sig += [] {};
		sig(1);
		//todo: support such usage sig -= nullptr;
		testSame(
			MapParamId<void(double,char),void(double, char, const std::string&)>,
			IdSeq<0,1>,
			ReplaceParam<Seq<int>,double>,
			Seq<double>
		);
	}
}