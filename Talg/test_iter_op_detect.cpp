#include "type_traits.h"
#include "core.h"
#include "iter_op_detect.h"

using namespace Talg;




namespace {
	
	struct A {
		void operator++(){}
	};
	struct B{
	};

	void f() {
		static_assert(hasPreIncOp<A>(0), "!");
		static_assert(!hasPreIncOp<B>(0), "!");
	}

}
