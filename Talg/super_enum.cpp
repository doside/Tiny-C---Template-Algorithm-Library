#include "ctstring.h"

enum Color {
	Red=0,
	Green,
	Blue=1<<(2*3+4-(12-5))
};

#define super_enum_m(name,...) 				\
[](auto...arg) {					\
	constexpr auto value = ...;		\
									\
}



