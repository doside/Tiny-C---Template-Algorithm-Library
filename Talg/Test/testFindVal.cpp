#include <Talg/find_val.h>
namespace {

using namespace Talg;


void f() {
	using cnt_t = CountType_sv<int, Seq<int, float, int, char, int, int>>;
	static_assert(staticCheck<cnt_t::value == 4, Tagi<cnt_t::value>>(), "");
}


}

