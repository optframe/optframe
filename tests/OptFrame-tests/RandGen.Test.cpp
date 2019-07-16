#include <gtest/gtest.h>

// system
#include <limits>

// core includes
#include <RandGen.hpp>
#include <Util/RandGenMersenneTwister.hpp>

using namespace std;
using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST(OptFrameUtilsTests, RandGen_20000_0_9_Is_Balanced_1_percent)
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
   EXPECT_LE(max - min, 0.01 * 20000);
}

TEST(OptFrameUtilsTests, RandGen_20000_10_Is_Balanced_1_percent)
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
   EXPECT_LE(max - min, 0.01 * 20000);
}


TEST(OptFrameUtilsTests, RandGenMT_20000_10_Is_Balanced_1_percent)
{
   std::map<int, int> hist;
   RandGenMersenneTwister rg;
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
   EXPECT_LE(max - min, 0.01 * 20000);
}
