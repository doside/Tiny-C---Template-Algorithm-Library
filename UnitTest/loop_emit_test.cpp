#include <doctest/doctest.h>
//#include <boost/signals2.hpp>
#include <Talg/signal_wrapper.h>
#include <Talg/slotlist.h>
#include <sstream>
#include <algorithm>
using namespace Talg;
TEST_CASE("Loop Emit Test") {
	//SignalWrapper<boost::signals2::signal,void()> sig;
	SimpleSignal<void()> sig;
	std::ostringstream os;
	int count = 3;
	auto prevent_loop=[](auto first, auto last,auto& sig) {
		auto beg=makeLockedIter(first, sig);
		auto end=makeLockedIter(last, sig);
		for (; beg != end; ++beg) {
			if (beg) {
				*beg;
			}
		}
	};
	auto con=sig + [&] {
			os << "a "<<count<<" ";
			if (count--) {
				os <<count<<" ";
				sig.collect(prevent_loop);
			}
			os << "A "<<count<<" ";
	};
	auto con2=sig + [&] {
			os << "b "<<count<<" ";
			if (count--) {
				os <<count<<" ";
				sig.collect(prevent_loop);
			}
			os << "B " << count<<" ";
	};
	sig.collect(prevent_loop);
	CHECK(os.str()=="a 3 2 b 2 1 B 1 A 1 b 1 0 a 0 A -1 B -1 ");

}

TEST_CASE("test multiple same parameter") {
	SimpleSignal<void(int,int,int,int)> sig;
	sig += [](int a, int b) {
		CHECK(a == 1);
		CHECK(b == 2);
	};
	sig(1, 2, 3, 4);
}