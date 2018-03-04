#include <Talg/core.h>
#include "test_suits.h"
#include <tuple>
#include <Talg/seqop.h>
#include <Talg/find_val.h>

using namespace Talg;

namespace {

	void f() {
		
		testSame(
			Seq<int,double,char>,
			Reverse<std::tuple<char,double,int>>,

			Seq<int, int, int, int>,
			Erase_front<Seq<int, double, float>, Seq<int, int, int, int, int>>,

			Seq<int, double, float>,
			Erase_front<Seq<bool, char>, Seq<int, char, double, float>>,

			Seq<>,
			Erase_front<Seq<int, double>, Seq<int>>,

			Seq<float>,
			Erase_front<Seq<int, double>, std::tuple<float>>,

			Seq<int, int, int, int>,
			Erase_front_s<Seq<int, double, float>, Seq<int, int, int, int, int>>,

			Seq<int, double, float>,
			Erase_front_s<Seq<bool, char>, Seq<int, char, double, float>>,

			Seq<>,
			Erase_front_s<Seq<int, double>, Seq<int>>,

			Seq<float>,
			Erase_front_s<Seq<int, double>, Seq<float>>,

			Erase_back<Seq<int,double>,Seq<bool,bool,bool>>,
			Seq<bool, bool, bool>,

			Erase_back<Seq<int,double>, Seq<bool, bool, bool,double>>,
			Seq<bool, bool, bool>,

			Erase_back<Seq<int, double>, Seq<bool, bool, bool,int,double>>,
			Seq<bool, bool, bool>,

			Seq<char,char>,
			Erase_back<Seq<>,Seq<char,char>>,

			Seq<char>,
			Erase_back<Seq<char>,Seq<char,char>>,

			Seq<char>,
			Erase_back<Seq<>,Seq<char>>,

			Seq<int,float,int,double>,
			EraseAt_s<2,Seq<int,float,float,int,double>>
		
		);



			
	}












}