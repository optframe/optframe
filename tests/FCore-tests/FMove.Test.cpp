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

auto barFunc = [] ( int n ) -> bool { return true; };
template<class X>
class S
{
   
};

using Y = S<[](int x) -> void { std::cout << x << " hello\n"; }>;

TEST_CASE("OptFrame FCore FMove: Create FMove")
{
   auto a1 = Foo<fooFunc>();
   //auto a2 = Foo< std::add_pointer<decltype(barFunc)>::type(0) >();
   auto a2 = Foo< decltype(barFunc) >();
   //
   //
   using MyXSolution = std::vector<int>;
   using MyXEvaluation = Evaluation<int>;
   using MyXESolution = std::pair<MyXSolution, MyXEvaluation>;

   std::pair<int, int> p(2, 3);
   //std::function<bool (const MyXESolution&)> fApply;
   auto fApply = [](const MyXESolution&) -> bool {
      return false;
   };

   FMove<std::pair<int, int>, MyXESolution, fApply>
     fmove(p);

   //Matrix<int> m(2);
   //REQUIRE(m.getNumRows() == 2);
   REQUIRE(true);
}
