#include <Talg/guard.h>
#include <Talg/makeit.h>
//#include <Talg/index_seq.h>
namespace {
	using namespace Talg;
	void f() {
		//auto a = makeit<Guard<>>([]()noexcept {});
		//auto b = makeit<Guard<>>([](){},weak_except_t{});
		//auto c = makeit2<Guard>([] {}, with_check_t{});
		auto d = makeGuard([] {}, weak_except_t{});
		auto e = makeGuard([] {}, with_check_t{});
		auto f = makeGuard([]()noexcept{});
	}
}