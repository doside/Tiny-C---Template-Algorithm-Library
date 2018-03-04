#include <type_traits>
#include <Talg/strip_qualifier.h>
#include "test_suits.h"

using namespace std;


using namespace Talg;


typedef void(*emptyfptr)();

typedef emptyfptr(*fptr_fptr)(int*(*)(emptyfptr));

struct A
{
	void f(int);
	int f2();
	static int f3();
};
using pmd = int(A::*)();

namespace {
	int f() {
		testSame(RemoveCvrp<fptr_fptr>, emptyfptr (int*(*)(emptyfptr)));
		testSame(RemoveCvrp<fptr_fptr>, RemoveCvrp<emptyfptr(int*(*)(emptyfptr))>);
		testSame(RemoveCvrp<pmd (A::*)()>, pmd());
		testSame(RemoveCvrp<int(*)()>, int());


		testSame(remove_pointer_t<void(*)(int)>, void(int));
		testSame(rm_top<void(*const)(int)>, void(int));
		testSame(rm_top<void(*const volatile)(int)>, void(int));
		testSame(rm_top<void(*const &)(int)>, void(int));
		testSame(rm_top<void(*const &&)(int)>, void(int));

		using cfptr = void(*const)(int);
		using cfptr_ref = cfptr&;
		using cfptr_cref = const cfptr&;

		testSame(rm_top< const int*const>, int);
		testSame(rm_top<cfptr>, void(int));
		testSame(rm_top<cfptr_ref>, void(int));
		testSame(rm_top<cfptr_cref>, void(int));
		testSame( RemoveCvrp<void(A::*const)(int)>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int)const>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int)const&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int)const&&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int)const volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int)const volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int)const volatile&&>,
			void(int) );

		testSame( RemoveCvrp<void(A::*const &)(int) volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int) volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const &)(int) volatile&&>,
			void(int) );


		testSame( RemoveCvrp<void(A::*&)(int)const>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int)const&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int)const&&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int)const volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int)const volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int)const volatile&&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int) volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int) volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&)(int) volatile&&>,
			void(int) );

		testSame( RemoveCvrp<void(A::*&&)(int)const>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&&)(int)const&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&&)(int)const&&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&&)(int)const volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&&)(int)const volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&&)(int)const volatile&&>,
			void(int) );

		testSame( RemoveCvrp<void(A::*&&)(int) volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&&)(int) volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*&&)(int) volatile&&>,
			void(int) );


		testSame( RemoveCvrp<void(A::*const)(int)const>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const)(int)const&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const)(int)const&&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const)(int)const volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const)(int)const volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const)(int)const volatile&&>,
			void(int) );

		testSame( RemoveCvrp<void(A::*const)(int) volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const)(int) volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const)(int) volatile&&>,
			void(int) );


		testSame( RemoveCvrp<void(A::*const volatile)(int)const>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const volatile)(int)const&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const volatile)(int)const&&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const volatile)(int)const volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const volatile)(int)const volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const volatile)(int)const volatile&&>,
			void(int) );

		testSame( RemoveCvrp<void(A::*const volatile)(int) volatile>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const volatile)(int) volatile&>,
			void(int) );
		testSame( RemoveCvrp<void(A::*const volatile)(int) volatile&&>,
			void(int) );

		testSame( RemoveCvrp<int(A::*const volatile)>,
			int() );
		testSame( RemoveCvrp<int(A::*const volatile)()>,
			int() );
		testSame( RemoveCvrp<decltype(&A::f3)>,
			int() );
		return 0;

	}
}
