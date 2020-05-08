#include <catch2/catch.hpp>
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md

// system
#include <limits>

// core includes
#include <OptFrame/RandGen.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>

using namespace std;
using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST_CASE("OptFrameUtilsTests: RandGen_20000_0_9_Is_Balanced_1_percent")
{
   std::map<int, int> hist;
   RandGen rg;
   for (int n = 0; n < 20000; ++n) {
      ++hist[rg.rand(0, 9)];
   }

   int min = 20000;
   int max = 0;
   for (int n = 0; n < 9; ++n) {
      if (hist[n] < min)
         min = hist[n];
      if (hist[n] > max)
         max = hist[n];
   }

   //for (auto p : hist) {
   //   std::cout << p.first << " : " << std::string(p.second / 100, '*') << '\n';
   //}

   //cout << "diff=" << (max-min)/20000.0 << endl;
   // MUST HAVE LESS THAN 1 PERCENT DEVIATION
   REQUIRE(max - min <= 0.01 * 20000);
}

TEST_CASE("OptFrameUtilsTests: RandGen_20000_10_Is_Balanced_1_percent")
{
   std::map<int, int> hist;
   RandGen rg;
   for (int n = 0; n < 20000; ++n) {
      ++hist[rg.rand(10)]; // 0 to 9
   }

   int min = 20000;
   int max = 0;
   for (int n = 0; n < 9; ++n) {
      if (hist[n] < min)
         min = hist[n];
      if (hist[n] > max)
         max = hist[n];
   }

   //for (auto p : hist) {
   //   std::cout << p.first << " : " << std::string(p.second / 100, '*') << '\n';
   //}

   //cout << "diff=" << (max-min)/20000.0 << endl;
   // MUST HAVE LESS THAN 1 PERCENT DEVIATION
   REQUIRE(max - min <= 0.01 * 20000);
}


TEST_CASE("OptFrameUtilsTests: RandGenMT_20000_10_Is_Balanced_1_percent")
{
   std::map<int, int> hist;
   RandGenMersenneTwister rg1;
   RandGen& rg = rg1;
   for (int n = 0; n < 20000; ++n) {
      ++hist[rg.rand(10)]; // 0 to 9
   }

   int min = 20000;
   int max = 0;
   for (int n = 0; n < 9; ++n) {
      if (hist[n] < min)
         min = hist[n];
      if (hist[n] > max)
         max = hist[n];
   }

   //for (auto p : hist) {
   //   std::cout << p.first << " : " << std::string(p.second / 100, '*') << '\n';
   //}

   //cout << "diff=" << (max-min)/20000.0 << endl;
   // MUST HAVE LESS THAN 1 PERCENT DEVIATION
   REQUIRE(max - min <= 0.01 * 20000);
}
