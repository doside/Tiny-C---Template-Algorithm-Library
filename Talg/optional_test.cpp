// Copyright (C) 2011 - 2016 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal

#include <doctest/doctest.h>
#include <Talg/optional.h>
#include <Talg/utility.h>
# include <vector>
# include <iostream>
# include <functional>
# include <complex>
struct caller {
    template <class T> caller(T fun) { fun(); }
};
# define CAT2(X, Y) X ## Y
# define CAT(X, Y) CAT2(X, Y)
# define ADD_TEST(NAME) TEST_CASE(#NAME)

enum  State 
{
    sDefaultConstructed,
    sValueCopyConstructed,
    sValueMoveConstructed,
    sCopyConstructed,
    sMoveConstructed,
    sMoveAssigned,
    sCopyAssigned,
    sValueCopyAssigned,
    sValueMoveAssigned,
    sMovedFrom,
    sValueConstructed
};

struct OracleVal
{
    State s;
    int i;
    OracleVal(int i = 0) : s(sValueConstructed), i(i) {}
};

struct Oracle
{
    State s;
    OracleVal val;

    Oracle() : s(sDefaultConstructed) {}
    Oracle(const OracleVal& v) : s(sValueCopyConstructed), val(v) {}
    Oracle(OracleVal&& v) : s(sValueMoveConstructed), val(std::move(v)) {v.s = sMovedFrom;}
    Oracle(const Oracle& o) : s(sCopyConstructed), val(o.val) {}
    Oracle(Oracle&& o) : s(sMoveConstructed), val(std::move(o.val)) {o.s = sMovedFrom;}

    Oracle& operator=(const OracleVal& v) { s = sValueCopyConstructed; val = v; return *this; }
    Oracle& operator=(OracleVal&& v) { s = sValueMoveConstructed; val = std::move(v); v.s = sMovedFrom; return *this; }
    Oracle& operator=(const Oracle& o) { s = sCopyConstructed; val = o.val; return *this; }
    Oracle& operator=(Oracle&& o) { s = sMoveConstructed; val = std::move(o.val); o.s = sMovedFrom; return *this; }
};

struct Guard
{
    std::string val;
    Guard() : val{} {}
    explicit Guard(std::string s, int = 0) : val(s) {}
    Guard(const Guard&) = delete;
    Guard(Guard&&) = delete;
    void operator=(const Guard&) = delete;
    void operator=(Guard&&) = delete;
};

struct ExplicitStr
{
    std::string s;
    explicit ExplicitStr(const char* chp) : s(chp) {};
};

struct Date
{
    int i;
    Date() = delete;
    Date(int i) : i{i} {};
    Date(Date&& d) : i(d.i) { d.i = 0; }
    Date(const Date&) = delete;
    Date& operator=(const Date&) = delete;
    Date& operator=(Date&& d) { i = d.i; d.i = 0; return *this;};
};



bool operator==( Oracle const& a, Oracle const& b ) { return a.val.i == b.val.i; }
bool operator!=( Oracle const& a, Oracle const& b ) { return a.val.i != b.val.i; }


namespace tr2 = Talg;


ADD_TEST(disengaged_ctor)
{
    Talg::optional<int> o1;
    CHECK (!o1);

    Talg::optional<int> o2 = tr2::nullopt;
    CHECK (!o2);

    Talg::optional<int> o3 = o2;
    CHECK (!o3);

    CHECK (o1 == tr2::nullopt);
    CHECK (o1 == Talg::optional<int>{});
    CHECK (!o1);
    CHECK (bool(o1) == false);

    CHECK (o2 == tr2::nullopt);
    CHECK (o2 == Talg::optional<int>{});
    CHECK (!o2);
    CHECK (bool(o2) == false);

    CHECK (o3 == tr2::nullopt);
    CHECK (o3 == Talg::optional<int>{});
    CHECK (!o3);
    CHECK (bool(o3) == false);

    CHECK (o1 == o2);
    CHECK (o2 == o1);
    CHECK (o1 == o3);
    CHECK (o3 == o1);
    CHECK (o2 == o3);
    CHECK (o3 == o2);
	CHECK (o1 <= o2);
	CHECK (o2 <= o1);
	CHECK (o1 <= o3);
	CHECK (o3 <= o1);
	CHECK (o2 <= o3);
	CHECK (o3 <= o2);
	CHECK (o1 >= o2);
	CHECK (o2 >= o1);
	CHECK (o1 >= o3);
	CHECK (o3 >= o1);
	CHECK (o2 >= o3);
	CHECK (o3 >= o2);
	CHECK (!(o1 != o2));
    CHECK (!(o2 != o1));
    CHECK (!(o1 != o3));
    CHECK (!(o3 != o1));
    CHECK (!(o2 != o3));
    CHECK (!(o3 != o2));

	CHECK (!(o1 < o2));
	CHECK (!(o2 < o1));
	CHECK (!(o1 < o3));
	CHECK (!(o3 < o1));
	CHECK (!(o2 < o3));
	CHECK (!(o3 < o2));

	CHECK (!(o1 > o2));
	CHECK (!(o2 > o1));
	CHECK (!(o1 > o3));
	CHECK (!(o3 > o1));
	CHECK (!(o2 > o3));
	CHECK (!(o3 > o2));
};


ADD_TEST(value_ctor)
{
	OracleVal v;
	Talg::optional<Oracle> oo1(v);
	CHECK (oo1 != tr2::nullopt);
	CHECK (oo1 != Talg::optional<Oracle>{});
	CHECK (oo1 == Talg::optional<Oracle>{v});
	CHECK (!!oo1);
	CHECK (bool(oo1));
	// NA: CHECK (oo1->s == sValueCopyConstructed);
	CHECK (oo1->s == sMoveConstructed);
	CHECK (v.s == sValueConstructed);
  
	Talg::optional<Oracle> oo2(std::move(v));
	CHECK (oo2 != tr2::nullopt);
	CHECK (oo2 != Talg::optional<Oracle>{});
	CHECK (oo2 == oo1);
	CHECK (!!oo2);
	CHECK (bool(oo2));
	// NA: CHECK (oo2->s == sValueMoveConstructed);
	CHECK (oo2->s == sMoveConstructed);
	CHECK (v.s == sMovedFrom);

	{
		OracleVal v;
		Talg::optional<Oracle> oo1{tr2::in_place, v};
		CHECK (oo1 != tr2::nullopt);
		CHECK (oo1 != Talg::optional<Oracle>{});
		CHECK (oo1 == Talg::optional<Oracle>{v});
		CHECK (!!oo1);
		CHECK (bool(oo1));
		CHECK (oo1->s == sValueCopyConstructed);
		CHECK (v.s == sValueConstructed);

		Talg::optional<Oracle> oo2{tr2::in_place, std::move(v)};
		CHECK (oo2 != tr2::nullopt);
		CHECK (oo2 != Talg::optional<Oracle>{});
		CHECK (oo2 == oo1);
		CHECK (!!oo2);
		CHECK (bool(oo2));
		CHECK (oo2->s == sValueMoveConstructed);
		CHECK (v.s == sMovedFrom);
	}
};


ADD_TEST(assignment)
{
    Talg::optional<int> oi;
    oi = Talg::optional<int>{1};
    CHECK (*oi == 1);

    oi = tr2::nullopt;
    CHECK (!oi);

    oi = 2;
    CHECK (*oi == 2);

    oi = {};
    CHECK (!oi);
};


template <class T>
struct MoveAware
{
  T val;
  bool moved;
  MoveAware(T val) : val(val), moved(false) {}
  MoveAware(MoveAware const&) = delete;
  MoveAware(MoveAware&& rhs) : val(rhs.val), moved(rhs.moved) {
    rhs.moved = true;
  }
  MoveAware& operator=(MoveAware const&) = delete;
  MoveAware& operator=(MoveAware&& rhs) {
    val = (rhs.val);
    moved = (rhs.moved);
    rhs.moved = true;
    return *this;
  }
};

ADD_TEST(moved_from_state)
{
  // first, test mock:
  MoveAware<int> i{1}, j{2};
  CHECK (i.val == 1);
  CHECK (!i.moved);
  CHECK (j.val == 2);
  CHECK (!j.moved);
  
  MoveAware<int> k = std::move(i);
  CHECK (k.val == 1);
  CHECK (!k.moved);
  CHECK (i.val == 1);
  CHECK (i.moved);
  
  k = std::move(j);
  CHECK (k.val == 2);
  CHECK (!k.moved);
  CHECK (j.val == 2);
  CHECK (j.moved);
  
  // now, test optional
  Talg::optional<MoveAware<int>> oi{1}, oj{2};
  CHECK (oi);
  CHECK (!oi->moved);
  CHECK (oj);
  CHECK (!oj->moved);
  
  Talg::optional<MoveAware<int>> ok = std::move(oi);
  CHECK (ok);
  CHECK (!ok->moved);
  CHECK (oi);
  CHECK (oi->moved);
  
  ok = std::move(oj);
  CHECK (ok);
  CHECK (!ok->moved);
  CHECK (oj);
  CHECK (oj->moved);
};


ADD_TEST(copy_move_ctor_optional_int)
{
  Talg::optional<int> oi;
  Talg::optional<int> oj = oi;
  
  CHECK (!oj);
  CHECK (oj == oi);
  CHECK (oj == tr2::nullopt);
  CHECK (!bool(oj));
  
  oi = 1;
  Talg::optional<int> ok = oi;
  CHECK (!!ok);
  CHECK (bool(ok));
  CHECK (ok == oi);
  CHECK (ok != oj);
  CHECK (*ok == 1);
  
  Talg::optional<int> ol = std::move(oi);
  CHECK (!!ol);
  CHECK (bool(ol));
  CHECK (ol == oi);
  CHECK (ol != oj);
  CHECK (*ol == 1);
};


ADD_TEST(optional_optional)
{
  Talg::optional<Talg::optional<int>> oi1 = tr2::nullopt;
  CHECK (oi1 == tr2::nullopt);
  CHECK (!oi1);
  
  {
  Talg::optional<Talg::optional<int>> oi2 {tr2::in_place};
  CHECK (oi2 != tr2::nullopt);
  CHECK (bool(oi2));
  CHECK (*oi2 == tr2::nullopt);
  //CHECK (!(*oi2));
  //std::cout << typeid(**oi2).name() << std::endl;
  }
  
  {
  Talg::optional<Talg::optional<int>> oi2 {tr2::in_place, tr2::nullopt};
  CHECK (oi2 != tr2::nullopt);
  CHECK (bool(oi2));
  CHECK (*oi2 == tr2::nullopt);
  CHECK (!*oi2);
  }
  
  {
  Talg::optional<Talg::optional<int>> oi2 {Talg::optional<int>{}};
  CHECK (oi2 != tr2::nullopt);
  CHECK (bool(oi2));
  CHECK (*oi2 == tr2::nullopt);
  CHECK (!*oi2);
  }
  
  Talg::optional<int> oi;
  auto ooi = tr2::make_optional(oi);
  static_assert( std::is_same<Talg::optional<Talg::optional<int>>, decltype(ooi)>::value, "");

};

ADD_TEST(example_guard)
{
  using namespace tr2;
  //FAILS: optional<Guard> ogx(Guard("res1")); 
  //FAILS: optional<Guard> ogx = "res1"; 
  //FAILS: optional<Guard> ogx("res1"); 
  optional<Guard> oga;                     // Guard is non-copyable (and non-moveable)     
  optional<Guard> ogb(in_place, "res1");   // initialzes the contained value with "res1"  
  CHECK (bool(ogb));
  CHECK (ogb->val == "res1");
            
  optional<Guard> ogc(in_place);           // default-constructs the contained value
  CHECK (bool(ogc));
  CHECK (ogc->val == "");

  oga.emplace("res1");                     // initialzes the contained value with "res1"  
  CHECK (bool(oga));
  CHECK (oga->val == "res1");
  
  oga.emplace();                           // destroys the contained value and 
                                           // default-constructs the new one
  CHECK (bool(oga));
  CHECK (oga->val == "");
  
  oga = nullopt;                        // OK: make disengaged the optional Guard
  CHECK (!(oga));
  //FAILS: ogb = {};                          // ERROR: Guard is not Moveable
};


void process(){}
void process(int ){}
void processNil(){}



ADD_TEST(example1)
{
  using namespace tr2;
  optional<int> oi;                 // create disengaged object
  optional<int> oj = nullopt;          // alternative syntax
  oi = oj;                          // assign disengaged object
  optional<int> ok = oj;            // ok is disengaged

  if (oi)  CHECK(false);           // 'if oi is engaged...'
  if (!oi) CHECK(true);            // 'if oi is disengaged...'

  if (oi != nullopt) CHECK(false);    // 'if oi is engaged...'
  if (oi == nullopt) CHECK(true);     // 'if oi is disengaged...'

  CHECK(oi == ok);                 // two disengaged optionals compare equal
  
  ///////////////////////////////////////////////////////////////////////////
  optional<int> ol{1};              // ol is engaged; its contained value is 1
  ok = 2;                           // ok becomes engaged; its contained value is 2
  oj = ol;                          // oj becomes engaged; its contained value is 1

  CHECK(oi != ol);                 // disengaged != engaged
  CHECK(ok != ol);                 // different contained values
  CHECK(oj == ol);                 // same contained value
  CHECK(oi < ol);                  // disengaged < engaged
  CHECK(ol < ok);                  // less by contained value
  
  /////////////////////////////////////////////////////////////////////////////
  optional<int> om{1};              // om is engaged; its contained value is 1
  optional<int> on = om;            // on is engaged; its contained value is 1
  om = 2;                           // om is engaged; its contained value is 2
  CHECK (on != om);                // on still contains 3. They are not pointers

  /////////////////////////////////////////////////////////////////////////////
  int i = *ol;                      // i obtains the value contained in ol
  CHECK (i == 1);
  *ol = 9;                          // the object contained in ol becomes 9
  CHECK(*ol == 9);
  CHECK(ol == make_optional(9));  
  
  ///////////////////////////////////
  int p = 1;
  optional<int> op = p;
  CHECK(*op == 1);
  p = 2;                         
  CHECK(*op == 1);                 // value contained in op is separated from p

  ////////////////////////////////
  if (ol)                      
    process(*ol);                   // use contained value if present
  else
    process();                      // proceed without contained value
    
  if (!om)   
    processNil();
  else  
    process(*om);   
  
  /////////////////////////////////////////
  process(ol.value_or(0));     // use 0 if ol is disengaged
  
  ////////////////////////////////////////////
  ok = nullopt;                         // if ok was engaged calls T's dtor
  oj = {};                           // assigns a temporary disengaged optional
};


ADD_TEST(example_guard) 
{
  using Talg::optional;
  const optional<int> c = 4; 
  int i = *c;                        // i becomes 4
  CHECK (i == 4);
  // FAILS: *c = i;                            // ERROR: cannot assign to const int&
};


template <typename T>
T getValue( Talg::optional<T> newVal = tr2::nullopt, Talg::optional<T&> storeHere = tr2::nullopt )
{
  T cached{};
  
  if (newVal) {
    cached = *newVal;
    
    if (storeHere) {
      *storeHere = *newVal; // LEGAL: assigning T to T
    }      
  }
  return cached;      
}

/*
ADD_TEST(example_optional_arg)
{
  int iii = 0;
  iii = getValue<int>(iii, iii);
  iii = getValue<int>(iii);
  iii = getValue<int>();
  
  {
    using namespace Talg;
    optional<Guard> grd1{in_place, "res1", 1};   // guard 1 initialized
    optional<Guard> grd2;

    grd2.emplace("res2", 2);                     // guard 2 initialized
    grd1 = nullopt;                                 // guard 1 released

  }                                              // guard 2 released (in dtor)
};
*/

std::tuple<Date, Date, Date> getStartMidEnd() { return std::tuple<Date, Date, Date>{Date{1}, Date{2}, Date{3}}; }
void run(Date const&, Date const&, Date const&) {}

ADD_TEST(example_date)
{
  using namespace Talg;
  optional<Date> start, mid, end;           // Date doesn't have default ctor (no good default date)

  std::tie(start, mid, end) = getStartMidEnd();
  run(*start, *mid, *end); 
};


Talg::optional<char> readNextChar(){ return{}; }

void run(Talg::optional<std::string>) {}
void run(std::complex<double>) {}


template <class T>
void assign_norebind(Talg::optional<T&>& optref, T& obj)
{
  if (optref) *optref = obj;
  else        optref.emplace(obj);
}

template <typename T> void unused(T&&) {}

ADD_TEST(example_conceptual_model)
{
  using namespace Talg;
  
  optional<int> oi = 0;
  optional<int> oj = 1;
  optional<int> ok = nullopt;

  oi = 1;
  oj = nullopt;
  ok = 0;

  unused(oi == nullopt);
  unused(oj == 0);
  unused(ok == 1);
};


ADD_TEST(example_rationale)
{
  using namespace Talg;
  if (optional<char> ch = readNextChar()) {
    unused(ch);
    // ...
  }
  
  //////////////////////////////////
  optional<int> opt1 = nullopt; 
  optional<int> opt2 = {}; 

  opt1 = nullopt;
  opt2 = {};

  if (opt1 == nullopt) {}
  if (!opt2) {}
  if (opt2 == optional<int>{}) {}
  
  
  
  ////////////////////////////////

  run(nullopt);            // pick the second overload
  // FAILS: run({});              // ambiguous

  if (opt1 == nullopt) {} // fine
  // FAILS: if (opt2 == {}) {}   // ilegal
  
  ////////////////////////////////
  CHECK (optional<unsigned>{}  < optional<unsigned>{0});
  CHECK (optional<unsigned>{0} < optional<unsigned>{1});
  CHECK (!(optional<unsigned>{}  < optional<unsigned>{}) );
  CHECK (!(optional<unsigned>{1} < optional<unsigned>{1}));

  CHECK (optional<unsigned>{}  != optional<unsigned>{0});
  CHECK (optional<unsigned>{0} != optional<unsigned>{1});
  CHECK (optional<unsigned>{}  == optional<unsigned>{} );
  CHECK (optional<unsigned>{0} == optional<unsigned>{0});
  
  /////////////////////////////////
  optional<int> o;
  o = make_optional(1);         // copy/move assignment
  o = 1;           // assignment from T
  o.emplace(1);    // emplacement 
  
  ////////////////////////////////////
  //int isas = 0, i = 9;
  //optional<int&> asas = i;
  //assign_norebind(asas, isas);
  
  /////////////////////////////////////
  ////Talg::optional<std::vector<int>> ov2 = {2, 3};
  ////CHECK (bool(ov2));
  ////CHECK ((*ov2)[1] == 3);
  ////
  ////////////////////////////////
  ////std::vector<int> v = {1, 2, 4, 8};
  ////optional<std::vector<int>> ov = {1, 2, 4, 8};

  ////CHECK (v == *ov);
  ////
  ////ov = {1, 2, 4, 8};

  ////std::allocator<int> a;
  ////optional<std::vector<int>> ou { in_place, {1, 2, 4, 8}, a };

  ////CHECK (ou == ov);

  //////////////////////////////
  // inconvenient syntax:
  {
    
      Talg::optional<std::vector<int>> ov2{tr2::in_place, {2, 3}};
    
      CHECK (bool(ov2));
      CHECK ((*ov2)[1] == 3);
  
      ////////////////////////////

      std::vector<int> v = {1, 2, 4, 8};
      optional<std::vector<int>> ov{tr2::in_place, {1, 2, 4, 8}};

      CHECK (v == *ov);
  
      ov.emplace({1, 2, 4, 8});
/*
      std::allocator<int> a;
      optional<std::vector<int>> ou { in_place, {1, 2, 4, 8}, a };

      CHECK (ou == ov);
*/
  }

  /////////////////////////////////
  {
  typedef int T;
  optional<optional<T>> ot {in_place};
  optional<optional<T>> ou {in_place, nullopt};
  optional<optional<T>> ov {optional<T>{}};
  
  optional<int> oi;
  auto ooi = make_optional(oi);
  static_assert( std::is_same<optional<optional<int>>, decltype(ooi)>::value, "");
  }
};


bool fun(std::string , Talg::optional<int> oi = Talg::nullopt) 
{
  return bool(oi);
}

ADD_TEST(example_converting_ctor)
{
  using namespace Talg;
  
  CHECK (true == fun("dog", 2));
  CHECK (false == fun("dog"));
  CHECK (false == fun("dog", nullopt)); // just to be explicit
};


ADD_TEST(bad_comparison)
{
  Talg::optional<int> oi, oj;
  int i;
  bool b = (oi == oj);
  b = (oi >= i);
  b = (oi == i);
  unused(b);
};


//// NOT APPLICABLE ANYMORE
////ADD_TEST(perfect_ctor)
////{
////  //Talg::optional<std::string> ois = "OS";
////  CHECK (*ois == "OS");
////  
////  // FAILS: Talg::optional<ExplicitStr> oes = "OS"; 
////  Talg::optional<ExplicitStr> oes{"OS"};
////  CHECK (oes->s == "OS");
////};

ADD_TEST(value_or)
{
  Talg::optional<int> oi = 1;
  int i = oi.value_or(0);
  CHECK (i == 1);
  
  oi = tr2::nullopt;
  CHECK (oi.value_or(3) == 3);
  
  Talg::optional<std::string> os{"AAA"};
  CHECK (os.value_or("BBB") == "AAA");
  os = {};
  CHECK (os.value_or("BBB") == "BBB");
};

ADD_TEST(reset)
{
  using namespace Talg;
  optional<int> oi {1};
  oi.reset();
  CHECK (!oi);

  int i = 1;
  //optional<const int&> oir {i};
  //oir.reset();
  //CHECK (!oir);
};

ADD_TEST(mixed_order)
{
  using namespace Talg;
  
  optional<int> oN {nullopt};
  optional<int> o0 {0};
  optional<int> o1 {1};
  
  CHECK ( (oN <   0));
  CHECK ( (oN <   1));
  CHECK (!(o0 <   0));
  CHECK ( (o0 <   1));
  CHECK (!(o1 <   0));
  CHECK (!(o1 <   1));
  
  CHECK (!(oN >=  0));
  CHECK (!(oN >=  1));
  CHECK ( (o0 >=  0));
  CHECK (!(o0 >=  1));
  CHECK ( (o1 >=  0));
  CHECK ( (o1 >=  1));
  
  CHECK (!(oN >   0));
  CHECK (!(oN >   1));
  CHECK (!(o0 >   0));
  CHECK (!(o0 >   1));
  CHECK ( (o1 >   0));
  CHECK (!(o1 >   1));
  
  CHECK ( (oN <=  0));
  CHECK ( (oN <=  1));
  CHECK ( (o0 <=  0));
  CHECK ( (o0 <=  1));
  CHECK (!(o1 <=  0));
  CHECK ( (o1 <=  1));
  
  CHECK ( (0 >  oN));
  CHECK ( (1 >  oN));
  CHECK (!(0 >  o0));
  CHECK ( (1 >  o0));
  CHECK (!(0 >  o1));
  CHECK (!(1 >  o1));
  
  CHECK (!(0 <= oN));
  CHECK (!(1 <= oN));
  CHECK ( (0 <= o0));
  CHECK (!(1 <= o0));
  CHECK ( (0 <= o1));
  CHECK ( (1 <= o1));
  
  CHECK (!(0 <  oN));
  CHECK (!(1 <  oN));
  CHECK (!(0 <  o0));
  CHECK (!(1 <  o0));
  CHECK ( (0 <  o1));
  CHECK (!(1 <  o1));
  
  CHECK ( (0 >= oN));
  CHECK ( (1 >= oN));
  CHECK ( (0 >= o0));
  CHECK ( (1 >= o0));
  CHECK (!(0 >= o1));
  CHECK ( (1 >= o1));
};

struct BadRelops
{
  int i;
};

constexpr bool operator<(BadRelops a, BadRelops b) { return a.i < b.i; }
constexpr bool operator>(BadRelops a, BadRelops b) { return a.i < b.i; } // intentional error!



ADD_TEST(bad_relops)
{
  using namespace Talg;
  BadRelops a{1}, b{2};
  CHECK (a < b);
  CHECK (a > b);
  
  optional<BadRelops> oa = a, ob = b;
  CHECK (oa < ob);
  CHECK (!(oa > ob));
  
  CHECK (oa < b);
  CHECK (oa > b);
  
  //optional<BadRelops&> ra = a, rb = b;
 //CHECK (ra < rb);
 //CHECK (!(ra > rb));
 //
 //CHECK (ra < b);
 //CHECK (ra > b);
};


ADD_TEST(mixed_equality)
{
  using namespace Talg;
  
  CHECK (make_optional(0) == 0);
  CHECK (make_optional(1) == 1);
  CHECK (make_optional(0) != 1);
  CHECK (make_optional(1) != 0);
  
  optional<int> oN {nullopt};
  optional<int> o0 {0};
  optional<int> o1 {1};
  
  CHECK (o0 ==  0);
  CHECK ( 0 == o0);
  CHECK (o1 ==  1);
  CHECK ( 1 == o1);
  CHECK (o1 !=  0);
  CHECK ( 0 != o1);
  CHECK (o0 !=  1);
  CHECK ( 1 != o0);
  
  CHECK ( 1 != oN);
  CHECK ( 0 != oN);
  CHECK (oN !=  1);
  CHECK (oN !=  0);
  CHECK (!( 1 == oN));
  CHECK (!( 0 == oN));
  CHECK (!(oN ==  1));
  CHECK (!(oN ==  0));
  
  std::string cat{"cat"}, dog{"dog"};
  optional<std::string> oNil{}, oDog{"dog"}, oCat{"cat"};
  
  CHECK (oCat ==  cat);
  CHECK ( cat == oCat);
  CHECK (oDog ==  dog);
  CHECK ( dog == oDog);
  CHECK (oDog !=  cat);
  CHECK ( cat != oDog);
  CHECK (oCat !=  dog);
  CHECK ( dog != oCat);
  
  CHECK ( dog != oNil);
  CHECK ( cat != oNil);
  CHECK (oNil !=  dog);
  CHECK (oNil !=  cat);
  CHECK (!( dog == oNil));
  CHECK (!( cat == oNil));
  CHECK (!(oNil ==  dog));
  CHECK (!(oNil ==  cat));
};

ADD_TEST(const_propagation)
{
  using namespace Talg;
  
  optional<int> mmi{0};
  static_assert(std::is_same<decltype(*mmi), int&>::value, "WTF");
  
  const optional<int> cmi{0};
  static_assert(std::is_same<decltype(*cmi), const int&>::value, "WTF");
  
  optional<const int> mci{0};
  static_assert(std::is_same<decltype(*mci), const int&>::value, "WTF");
  
  optional<const int> cci{0};
  static_assert(std::is_same<decltype(*cci), const int&>::value, "WTF");
};


static_assert(std::is_base_of<std::logic_error, Talg::bad_optional_access>::value, "");

ADD_TEST(safe_value)
{
  using namespace Talg;
  
  try {
    optional<int> ovN{}, ov1{1};
    
    int& r1 = ov1.value();
    CHECK (r1 == 1);
    
    try { 
      ovN.value();
      CHECK (false);
    }
    catch (bad_optional_access const&) {
    }
    
    /*{ // ref variant
      int i1 = 1;
      //optional<int&> orN{}, or1{i1};
      
      int& r2 = or1.value();
      CHECK (r2 == 1);
      
      try { 
        //orN.value();
        CHECK (false);
      }
      catch (bad_optional_access const&) {
      }
    }*/
  }  
  catch(...) {
    CHECK (false);
  }
};



ADD_TEST(optional_initialization)
{
    using namespace tr2;
    using std::string;
    string s = "STR";

    optional<string> os{s};
    optional<string> ot = s;
    optional<string> ou{"STR"};
    optional<string> ov = string{"STR"};
    
};

#include <unordered_set>

ADD_TEST(optional_hashing)
{
    using namespace tr2;
    using std::string;
    
    std::hash<int> hi;
    std::hash<optional<int>> hoi;
    std::hash<string> hs;
    std::hash<optional<string>> hos;
    
    CHECK (hi(0) == hoi(optional<int>{0}));
    CHECK (hi(1) == hoi(optional<int>{1}));
    CHECK (hi(3198) == hoi(optional<int>{3198}));
    
    CHECK (hs("") == hos(optional<string>{""}));
    CHECK (hs("0") == hos(optional<string>{"0"}));
    CHECK (hs("Qa1#") == hos(optional<string>{"Qa1#"}));
    
    std::unordered_set<optional<string>> set;
    CHECK(set.find({"Qa1#"}) == set.end());
    
    set.insert({"0"});
    CHECK(set.find({"Qa1#"}) == set.end());
    
    set.insert({"Qa1#"});
    CHECK(set.find({"Qa1#"}) != set.end());
};


// optional_ref_emulation
template <class T>
struct generic
{
  typedef T type;
};

template <class U>
struct generic<U&>
{
  typedef std::reference_wrapper<U> type;
};

template <class T>
using Generic = typename generic<T>::type;

template <class X>
bool generic_fun()
{
  Talg::optional<Generic<X>> op;
  return bool(op);
}

ADD_TEST(optional_ref_emulation)
{
  using namespace Talg;
  optional<Generic<int>> oi = 1;
  CHECK (*oi == 1);
  
  int i = 8;
  int j = 4;
  optional<Generic<int&>> ori {i};
  CHECK (*ori == 8);
  CHECK ((void*)&*ori != (void*)&i); // !DIFFERENT THAN optional<T&>

  *ori = j;
  CHECK (*ori == 4);
};


# if OPTIONAL_HAS_THIS_RVALUE_REFS == 1
ADD_TEST(moved_on_value_or)
{
  using namespace tr2;
  optional<Oracle> oo{in_place};
  
  CHECK (oo);
  CHECK (oo->s == sDefaultConstructed);
  
  Oracle o = std::move(oo).value_or( Oracle{OracleVal{}} );
  CHECK (oo);
  CHECK (oo->s == sMovedFrom);
  CHECK (o.s == sMoveConstructed);
  
  optional<MoveAware<int>> om {in_place, 1};
  CHECK (om);
  CHECK (om->moved == false);
  
  /*MoveAware<int> m =*/ std::move(om).value_or( MoveAware<int>{1} );
  CHECK (om);
  CHECK (om->moved == true);

# if OPTIONAL_HAS_MOVE_ACCESSORS == 1  
  {
    Date d = optional<Date>{in_place, 1}.value();
    CHECK (d.i); // to silence compiler warning
	
	Date d2 = *optional<Date>{in_place, 1};
    CHECK (d2.i); // to silence compiler warning
  }
# endif
};
# endif


struct Combined
{
  int m = 0;
  int n = 1;
  
  constexpr Combined() : m{5}, n{6} {}
  constexpr Combined(int m, int n) : m{m}, n{n} {}
};

struct Nasty
{
  int m = 0;
  int n = 1;
  
  constexpr Nasty() : m{5}, n{6} {}
  constexpr Nasty(int m, int n) : m{m}, n{n} {}
  
  int operator&() { return n; }
  int operator&() const { return n; }
};

ADD_TEST(arrow_operator)
{
  using namespace Talg;
  
  optional<Combined> oc1{in_place, 1, 2};
  CHECK (oc1);
  CHECK (oc1->m == 1);
  CHECK (oc1->n == 2);
  
  optional<Nasty> on{in_place, 1, 2};
  CHECK (on);
  CHECK (on->m == 1);
  CHECK (on->n == 2);
};

ADD_TEST(no_dangling_reference_in_value)
{
  // this mostly tests compiler warnings
  using namespace Talg;
  optional<int> oi {2};
  unused (oi.value());
  const optional<int> coi {3};
  unused (coi.value());
};

struct CountedObject
{
  static int _counter;
  bool _throw;
  CountedObject(bool b) : _throw(b) { ++_counter; }
  CountedObject(CountedObject const& rhs) : _throw(rhs._throw) { if (_throw) throw int(); }
  ~CountedObject() { --_counter; }
};

int CountedObject::_counter = 0;

ADD_TEST(exception_safety)
{
  using namespace Talg;
  try {
    optional<CountedObject> oo(in_place, true); // throw
    optional<CountedObject> o1(oo);
  }
  catch(...)
  {
    //
  }
  CHECK(CountedObject::_counter == 0);
  
  try {
    optional<CountedObject> oo(in_place, true); // throw
    optional<CountedObject> o1(std::move(oo));  // now move
  }
  catch(...)
  {
    //
  }
  CHECK(CountedObject::_counter == 0);
};

ADD_TEST(nested_optional)
{
   using namespace Talg;
	
   optional<optional<optional<int>>> o1 {nullopt};
   CHECK (!o1);
    
   optional<optional<optional<int>>> o2 {in_place, nullopt};
   CHECK (o2);
   CHECK (!*o2);
    
   optional<optional<optional<int>>> o3 (in_place, in_place, nullopt);
   CHECK (o3);
   CHECK (*o3);
   CHECK (!**o3);
};

ADD_TEST(three_ways_of_having_value)
{
  using namespace Talg;
  optional<int> oN, o1 (1);
  
  CHECK (!oN);
  CHECK (!oN.has_value());
  CHECK (oN == nullopt);
  
  CHECK (o1);
  CHECK (o1.has_value());
  CHECK (o1 != nullopt);
  
  CHECK (bool(oN) == oN.has_value());
  CHECK (bool(o1) == o1.has_value());
  
  int i = 1;
  //optional<int&> rN, r1 (i);
  
  //CHECK (!rN);
  //CHECK (!rN.has_value());
  //CHECK (rN == nullopt);
  //
  //CHECK (r1);
  //CHECK (r1.has_value());
  //CHECK (r1 != nullopt);
  //
  //CHECK (bool(rN) == rN.has_value());
  //CHECK (bool(r1) == r1.has_value());
};

//// constexpr tests

// these 4 classes have different noexcept signatures in move operations
struct NothrowBoth {
  NothrowBoth(NothrowBoth&&) noexcept(true) {};
  void operator=(NothrowBoth&&) noexcept(true) {};
};
struct NothrowCtor {
  NothrowCtor(NothrowCtor&&) noexcept(true) {};
  void operator=(NothrowCtor&&) noexcept(false) {};
};
struct NothrowAssign {
  NothrowAssign(NothrowAssign&&) noexcept(false) {};
  void operator=(NothrowAssign&&) noexcept(true) {};
};
struct NothrowNone {
  NothrowNone(NothrowNone&&) noexcept(false) {};
  void operator=(NothrowNone&&) noexcept(false) {};
};

void test_noexcept()
{
  {
    Talg::optional<NothrowBoth> b1, b2;
    static_assert(noexcept(Talg::optional<NothrowBoth>{std::move(b1)}), "bad noexcept!");
    static_assert(noexcept(b1 = std::move(b2)), "bad noexcept!");
  }
  {
    Talg::optional<NothrowCtor> c1, c2;
    static_assert(noexcept(Talg::optional<NothrowCtor>{std::move(c1)}), "bad noexcept!");
    static_assert(!noexcept(c1 = std::move(c2)), "bad noexcept!");
  }
  {
    Talg::optional<NothrowAssign> a1, a2;
    static_assert(!noexcept(Talg::optional<NothrowAssign>{std::move(a1)}), "bad noexcept!");
    static_assert(!noexcept(a1 = std::move(a2)), "bad noexcept!");
  }
  {
    Talg::optional<NothrowNone> n1, n2;
    static_assert(!noexcept(Talg::optional<NothrowNone>{std::move(n1)}), "bad noexcept!");
    static_assert(!noexcept(n1 = std::move(n2)), "bad noexcept!");
  }
}


void constexpr_test_disengaged()
{
  constexpr Talg::optional<int> g0{};
  constexpr Talg::optional<int> g1{tr2::nullopt};
  static_assert( !g0, "initialized!" );
  static_assert( !g1, "initialized!" );
  
  static_assert( bool(g1) == bool(g0), "ne!" );
  
  static_assert( g1 == g0, "ne!" );
  static_assert( !(g1 != g0), "ne!" );
  static_assert( g1 >= g0, "ne!" );
  static_assert( !(g1 > g0), "ne!" );
  static_assert( g1 <= g0, "ne!" );
  static_assert( !(g1 <g0), "ne!" );
  
  static_assert( g1 == tr2::nullopt, "!" );
  static_assert( !(g1 != tr2::nullopt), "!" );
  static_assert( g1 <= tr2::nullopt, "!" );
  static_assert( !(g1 < tr2::nullopt), "!" );
  static_assert( g1 >= tr2::nullopt, "!" );
  static_assert( !(g1 > tr2::nullopt), "!" );
  
  static_assert(  (tr2::nullopt == g0), "!" );
  static_assert( !(tr2::nullopt != g0), "!" );
  static_assert(  (tr2::nullopt >= g0), "!" );
  static_assert( !(tr2::nullopt >  g0), "!" );
  static_assert(  (tr2::nullopt <= g0), "!" );
  static_assert( !(tr2::nullopt <  g0), "!" );
  
  static_assert(  (g1 != Talg::optional<int>(1)), "!" );
  static_assert( !(g1 == Talg::optional<int>(1)), "!" );
  static_assert(  (g1 <  Talg::optional<int>(1)), "!" );
  static_assert(  (g1 <= Talg::optional<int>(1)), "!" );
  static_assert( !(g1 >  Talg::optional<int>(1)), "!" );
  static_assert( !(g1 >  Talg::optional<int>(1)), "!" );
}


constexpr Talg::optional<int> g0{};
constexpr Talg::optional<int> g2{2};
static_assert( g2, "not initialized!" );
static_assert( *g2 == 2, "not 2!" );
static_assert( g2 == Talg::optional<int>(2), "not 2!" );
static_assert( g2 != g0, "eq!" );

# if OPTIONAL_HAS_MOVE_ACCESSORS == 1
static_assert( *Talg::optional<int>{3} == 3, "WTF!" );
static_assert( Talg::optional<int>{3}.value() == 3, "WTF!" );
static_assert( Talg::optional<int>{3}.value_or(1) == 3, "WTF!" );
static_assert( Talg::optional<int>{}.value_or(4) == 4, "WTF!" );
# endif

constexpr Talg::optional<Combined> gc0{tr2::in_place};
static_assert(gc0->n == 6, "WTF!");

// optional refs
int gi = 0;
//constexpr Talg::optional<int&> gori = gi;
//constexpr Talg::optional<int&> gorn{};
//constexpr int& gri = *gori;
//static_assert(gori, "WTF");
//static_assert(!gorn, "WTF");
//static_assert(gori != tr2::nullopt, "WTF");
//static_assert(gorn == tr2::nullopt, "WTF");
//static_assert(&gri == &*gori, "WTF");

constexpr int gci = 1;
//constexpr Talg::optional<int const&> gorci = gci;
//constexpr Talg::optional<int const&> gorcn{};

//static_assert(gorcn <  gorci, "WTF");
//static_assert(gorcn <= gorci, "WTF");
//static_assert(gorci == gorci, "WTF");
//static_assert(*gorci == 1, "WTF");
//static_assert(gorci == gci, "WTF");

namespace constexpr_optional_ref_and_arrow 
{
  using namespace Talg;
  constexpr Combined c{1, 2};
  //constexpr optional<Combined const&> oc = c;
  //static_assert(oc, "WTF!");
  //static_assert(oc->m == 1, "WTF!");
  //static_assert(oc->n == 2, "WTF!");
}



namespace InitList
{
  using namespace Talg;
  
  struct ConstInitLister
  {
    template <class...Ts>
	constexpr ConstInitLister(Tself<Ts,IsDecaySame<Ts,int>>&&...args) : len (sizeof...(args)) {}
	constexpr auto length()const { return len; }
    size_t len;
  };
  
  constexpr ConstInitLister CIL(2, 3, 4);
  static_assert(CIL.length() == 3, "WTF!");
  
  constexpr optional<ConstInitLister> oil {in_place, 4, 5, 6, 7};
  static_assert(oil, "WTF!");
  static_assert(oil->length() == 4, "WTF!");
}



// end constexpr tests


#include <string>


struct VEC
{
    std::vector<int> v;
    template <typename... X>
    VEC( X&&...x) : v(std::forward<X>(x)...) {}

    template <typename U, typename... X>
    VEC(std::initializer_list<U> il, X&&...x) : v(il, std::forward<X>(x)...) {}
};

#if 0


#if 0
ADD_TEST(example_ref)
{
  using namespace Talg;
  int i = 1;
  int j = 2;
  optional<int&> ora;                 // disengaged optional reference to int
  optional<int&> orb = i;             // contained reference refers to object i

  *orb = 3;                          // i becomes 3
  // FAILS: ora = j;                           // ERROR: optional refs do not have assignment from T
  // FAILS: ora = {j};                         // ERROR: optional refs do not have copy/move assignment
  // FAILS: ora = orb;                         // ERROR: no copy/move assignment
  ora.emplace(j);                    // OK: contained reference refers to object j
  ora.emplace(i);                    // OK: contained reference now refers to object i

  ora = nullopt;                        // OK: ora becomes disengaged
};


ADD_TEST(optional_ref)
{
  using namespace tr2;
  // FAILS: optional<int&&> orr;
  // FAILS: optional<nullopt_t&> on;
  int i = 8;
  optional<int&> ori;
  CHECK (!ori);
  ori.emplace(i);
  CHECK (bool(ori));
  CHECK (*ori == 8);
  CHECK (&*ori == &i);
  *ori = 9;
  CHECK (i == 9);
  
  // FAILS: int& ir = ori.value_or(i);
  int ii = ori.value_or(i);
  CHECK (ii == 9);
  ii = 7;
  CHECK (*ori == 9);
  
  int j = 22;
  auto&& oj = make_optional(std::ref(j));
  *oj = 23;
  CHECK (&*oj == &j);
  CHECK (j == 23);
};

ADD_TEST(optional_ref_const_propagation)
{
  using namespace Talg;
  
  int i = 9;
  const optional<int&> mi = i;
  int& r = *mi; 
  optional<const int&> ci = i;
  static_assert(std::is_same<decltype(*mi), int&>::value, "WTF");
  static_assert(std::is_same<decltype(*ci), const int&>::value, "WTF");
  
  unused(r);
};

ADD_TEST(optional_ref_assign)
{
  using namespace Talg;
  
  int i = 9;
  optional<int&> ori = i;
  
  int j = 1;
  ori = optional<int&>{j};
  ori = {j};
  // FAILS: ori = j;
  
  optional<int&> orx = ori;
  ori = orx;
  
  optional<int&> orj = j;
  
  CHECK (ori);
  CHECK (*ori == 1);
  CHECK (ori == orj);
  CHECK (i == 9);
  
  *ori = 2;
  CHECK (*ori == 2);
  CHECK (ori == 2);
  CHECK (2 == ori);
  CHECK (ori != 3);
  
  CHECK (ori == orj);
  CHECK (j == 2);
  CHECK (i == 9);
  
  ori = {};
  CHECK (!ori);
  CHECK (ori != orj);
  CHECK (j == 2);
  CHECK (i == 9);
};


ADD_TEST(optional_ref_swap)
{
  using namespace Talg;
  int i = 0;
  int j = 1;
  optional<int&> oi = i;
  optional<int&> oj = j;
  
  CHECK (&*oi == &i);
  CHECK (&*oj == &j);
  
  swap(oi, oj);
  CHECK (&*oi == &j);
  CHECK (&*oj == &i);
};



ADD_TEST(optional_ref_hashing)
{
    using namespace tr2;
    using std::string;
    
    std::hash<int> hi;
    std::hash<optional<int&>> hoi;
    std::hash<string> hs;
    std::hash<optional<string&>> hos;
    
    int i0 = 0;
    int i1 = 1;
    CHECK (hi(0) == hoi(optional<int&>{i0}));
    CHECK (hi(1) == hoi(optional<int&>{i1}));
    
    string s{""};
    string s0{"0"};
    string sCAT{"CAT"};
    CHECK (hs("") == hos(optional<string&>{s}));
    CHECK (hs("0") == hos(optional<string&>{s0}));
    CHECK (hs("CAT") == hos(optional<string&>{sCAT}));
    
    std::unordered_set<optional<string&>> set;
    CHECK(set.find({sCAT}) == set.end());
    
    set.insert({s0});
    CHECK(set.find({sCAT}) == set.end());
    
    set.insert({sCAT});
    CHECK(set.find({sCAT}) != set.end());
};


ADD_TEST(arrow_wit_optional_ref)
{
  using namespace Talg;
  
  Combined c{1, 2};
  optional<Combined&> oc = c;
  CHECK (oc);
  CHECK (oc->m == 1);
  CHECK (oc->n == 2);
  
  Nasty n{1, 2};
  Nasty m{3, 4};
  Nasty p{5, 6};
  
  optional<Nasty&> on{n};
  CHECK (on);
  CHECK (on->m == 1);
  CHECK (on->n == 2);
  
  on = {m};
  CHECK (on);
  CHECK (on->m == 3);
  CHECK (on->n == 4);
  
  on.emplace(p);
  CHECK (on);
  CHECK (on->m == 5);
  CHECK (on->n == 6);
  
  optional<Nasty&> om{in_place, n};
  CHECK (om);
  CHECK (om->m == 1);
  CHECK (om->n == 2);
};
#endif





#endif

struct TestSwapable {
	friend void swap(TestSwapable& lhs, TestSwapable& rhs)noexcept {

	}
	TestSwapable() {}
	TestSwapable(TestSwapable&&) {
		throw 0;
	}
	TestSwapable operator=(TestSwapable&&) { throw 0; }
};



ADD_TEST(Optionaltest) {
  Talg::optional<int> oi = 1;
  CHECK (bool(oi));
  oi.operator=({});
  CHECK (!oi);



  VEC v = {5, 6};


}


void test_swap() {
	TestSwapable l, r;
	static_assert(noexcept(std::swap(l, r))==false, "");
	static_assert(Talg::is_nothrow_swapable<TestSwapable>(), "");
}