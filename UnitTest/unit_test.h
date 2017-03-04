#ifndef UNITTEST_H_INCLUDED_
#define UNITTEST_H_INCLUDED_

#include <vector>
#include <functional>
#include <stdexcept>

struct TestRunner {
	static decltype(auto) get() {
		static std::vector<std::function<void()>> tests;
		return tests;
	}
	
	static void add(std::function<void()> func) {
		get().push_back(func);
	}
	static void run() {
		for (auto&elem : get()) {
			elem();
		}
	}
};

struct Test {
	Test(std::function<void()> func) {
		TestRunner::add(func);
	}
};

inline void require(bool expr) {
	if (!expr) {
		throw std::runtime_error{ "failed to require." };
	}
}




#endif	//UNITTEST_H_INCLUDED_
