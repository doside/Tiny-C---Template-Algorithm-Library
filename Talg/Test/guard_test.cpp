#include <Talg/guard.h>
#include <Talg/makeit.h>
#include <Talg/index_seq.h>
namespace {
	using namespace Talg;
	void f() {
		auto a = makeit<Guard<>>([]()noexcept {});
		auto b = makeit<Guard<>>([](){},weak_except_t{});
		auto c = makeit<Guard<>>([] {}, with_check_t{});
	}
}