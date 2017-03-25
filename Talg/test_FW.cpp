#include "function_wrapper.h"
#include <string>
#include "test_suits.h"
#include "slotlist.h"
namespace {
	using namespace Talg;

	void f() {
		SimpleSignal<void(), DefaultSlotTraits<void(),std::function<void()>>> sig;
		sig += [] {};
		//todo: support such usage sig -= nullptr;
		testSame(
			MapParamId<void(double,char),void(double, char, const std::string&)>,
			IdSeq<0,1>,
			ReplaceParam<Seq<int>,double>,
			Seq<double>
		);
	}
}