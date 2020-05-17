#include <catch2/catch.hpp>
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md

// system
#include <functional>

// core includes
#include <OptFCore/coro/Generator.hpp>

using namespace std;
using namespace optframe;

// coroutine tests with optframe::Generator

TEST_CASE("OptFrame FCore Generator: test basic fib max 6")
{
   // fib test based on: https://github.com/roger-dv/cpp20-coro-generator/
   auto fib = [](const double ceiling) -> Generator<double> {
      double j = 0;
      double i = 1;
      co_yield j;
      if (ceiling > j) {
         do {
            co_yield i;
            double tmp = i;
            i += j;
            j = tmp;
         } while (i <= ceiling);
      }
   };

   Generator<double> iter = fib(6); // or just 'auto'
   REQUIRE(iter.next());            // has 0
   REQUIRE(iter.getValue() == 0);
   REQUIRE(iter.next()); // has 1
   REQUIRE(iter.getValue() == 1);
   REQUIRE(iter.next()); // has 1
   REQUIRE(iter.getValue() == 1);
   REQUIRE(iter.next()); // has 2
   REQUIRE(iter.getValue() == 2);
   REQUIRE(iter.next()); // has 3
   REQUIRE(iter.getValue() == 3);
   REQUIRE(iter.next()); // has 5
   REQUIRE(iter.getValue() == 5);
   REQUIRE(!iter.next()); // finished
}

TEST_CASE("OptFrame FCore Generator: test pointer int returns")
{
   auto int_pointers = [](const int count) -> Generator<int*> {
      int i = 0;
      co_yield new int{ i };
      while (++i < count)
         co_yield new int{ i };
   };

   Generator<int*> iter = int_pointers(3); // or just 'auto'
   REQUIRE(iter.next());                   // has 0
   REQUIRE(*iter.getValue() == 0);
   REQUIRE(iter.next()); // has 1
   REQUIRE(*iter.getValue() == 1);
   REQUIRE(iter.next()); // has 1
   REQUIRE(*iter.getValue() == 2);
   REQUIRE(!iter.next()); // finished
}

// cannot make it work with 'uptr'.. better avoid!
/*
TEST_CASE("OptFrame FCore Generator: test uptr int returns")
{
   auto int_pointers = [](const int count) -> Generator<uptr<int>> {
      int i = 0;
      co_yield std::move(uptr<int>{ new int{ i } });
      while (++i < count)
         co_yield std::move(uptr<int>{ new int{ i } });
   };

   Generator<uptr<int>> iter = int_pointers(3); // or just 'auto'
   REQUIRE(iter.next());                        // has 0
   REQUIRE(*iter.getValue() == 0);
   REQUIRE(iter.next()); // has 1
   REQUIRE(*iter.getValue() == 1);
   REQUIRE(iter.next()); // has 1
   REQUIRE(*iter.getValue() == 2);
   REQUIRE(!iter.next()); // finished
}
*/