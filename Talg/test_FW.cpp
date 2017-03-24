#include "function_wrapper.h"
#include <string>
#include "test_suits.h"
namespace {
	using namespace Talg;
	void f() {
		
		testSame(
			MapParamId<void(double,char),void(double, char, const std::string&)>,
			IdSeq<0,1>
		);

	}
}