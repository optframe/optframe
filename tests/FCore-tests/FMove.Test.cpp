#include <catch2/catch.hpp>
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md

// system
#include <functional>

// core includes
#include <OptFCore/FMove.hpp>

using namespace std;
using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

template<bool (*F)(int)>
class Foo
{
public:
   Foo()
   {
      cout << F(10) << endl;
   }
};

bool
fooFunc(int n)
{
   return true;
}

auto barFunc = [](int n) -> bool { return true; };
//
template<void (*X)(int)>
class S
{
};

// TODO: funciona o Y?
//using Y = S< [](int x) -> void { std::cout << x << " hello\n"; } >;
using Y = S<+[](int x) -> void { std::cout << x << " hello\n"; }>;

//
//
using MyXSolution = Solution<std::vector<int>>;
using MyXESolution = std::pair<MyXSolution, Evaluation<int>>;
//

// this works when defining in global scope (same as 'class')
using MyMoveSwap = FMove<
  std::pair<int, int>,
  MyXESolution,
  [](const std::pair<int, int>& myData, MyXESolution& xes) -> std::pair<int, int> {
     return std::pair<int, int>(myData.second, myData.first);
  }>;

using myDS = std::pair<int, int>;
// this needs to be in global scope... too bad!
auto fCanBeApplied = [](const myDS&, const MyXESolution&) -> bool {
   return false;
};

auto fApply =
  [](const myDS& myData, MyXESolution& xes) -> myDS {
   return myDS(myData.second, myData.first);
};

// function requires "linkage", thus we define on global scope

TEST_CASE("OptFrame FCore FMove: Create FMove")
{
   auto a1 = Foo<fooFunc>();
   //auto a2 = Foo< std::add_pointer<decltype(barFunc)>::type(0) >();
   auto a2 = Foo<barFunc>();
   //
   //

   MyMoveSwap mswap(make_pair(3, 4));

   myDS p(2, 3);
   //std::function<bool (const MyXESolution&)> fApply;

   FMove<myDS, MyXESolution, fApply, fCanBeApplied> fmove{ p };

   FMove<
     myDS,
     MyXESolution,
     fApply>
     fmoveDefault{ p };

   //Matrix<int> m(2);
   //REQUIRE(m.getNumRows() == 2);
   REQUIRE(true);
}
