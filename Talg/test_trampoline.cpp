#include <Talg/constexpr_extend.h>
using namespace Talg;


namespace {
	
	struct F
	{
		constexpr F operator()()const { return F{}; }
		constexpr bool done()const { return false; }
	};
	
#define CAT2(name,line) name##line
#define CAT(name,line) CAT2(name,line)
#define LINE_VAR(name) CAT(name,__LINE__)

#define TEST auto LINE_VAR(var)=[]{   \
		constexpr auto res=trampoline(F{}); \
		return res;							\
	};

	///todo test compile speed.
	TEST
	TEST
	TEST

}