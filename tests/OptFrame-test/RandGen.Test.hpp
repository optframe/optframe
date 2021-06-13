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

TEST_CASE("OptFrameUtilsTests: RandGen_has_no_Min_Weakness")
{
   // for every seed in 0..11615 (I manually tested), result should be 'min' (interval of 10 elements)
   // explanation: https://stackoverflow.com/questions/21843172/c-uniform-int-distribution-always-returning-min-on-first-invocation

   std::vector<int> vcount0(11, 0);
   for (unsigned seed = 0; seed <= 11616; seed++) {
      default_random_engine engine(seed); // minstd_rand0
      uniform_int_distribution<int> distribution(20, 30);
      auto x = distribution(engine);
      //REQUIRE(x == 20); // ALL THE TIMES IT'S 20, EXCEPT LAST!
      vcount0[x-20]++;
   }

   //for(unsigned i=0; i<vcount0.size(); i++)
   //   std::cout << "vcount0[" << i << "]=" << vcount0[i] << std::endl;
   REQUIRE(vcount0[0]==11616);
   REQUIRE(vcount0[1]==1);
   REQUIRE(vcount0[2]==0); // SAME FOR REST...
   REQUIRE(vcount0[10]==0); // LAST

   // ============================
   // OptFrame Mersenne Twister
   // ============================

   std::vector<int> vcount1(11, 0);
   for (unsigned seed = 0; seed <= 11616; seed++) {
      RandGenMersenneTwister rg1(seed);
      //std::cout << "seed = " << seed << std::endl;
      //REQUIRE(rg1.rand(20, 30) != 20); // FOR SEED 0 IT'S NOT 'min'... strange!
      vcount1[rg1.rand(20, 30)-20]++;
   }

   for(unsigned i=0; i<vcount1.size(); i++)
      REQUIRE(vcount1[i] > 1000); // all over 1000 (~11617 / 11)
      //std::cout << "vcount1[" << i << "]=" << vcount1[i] << std::endl;
   

   // ===================================
   // OptFrame RandGen (with seed fix)
   // ===================================

   std::vector<int> vcount2(11, 0);
   for (unsigned seed = 0; seed <= 11616; seed++) {
      RandGen rg2(seed);
      //std::cout << "seed = " << seed << std::endl;
      //REQUIRE(rg1.rand(20, 30) != 20); // FOR SEED 0 IT'S NOT 'min'... strange!
      vcount2[rg2.rand(20, 30)-20]++;
   }

   for(unsigned i=0; i<vcount2.size(); i++)
      REQUIRE(vcount2[i] > 1000); // all over 1000 (~11617 / 11)
      //std::cout << "vcount2[" << i << "]=" << vcount2[i] << std::endl;
}
