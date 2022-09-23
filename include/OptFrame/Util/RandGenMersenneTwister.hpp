// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//Description and pseudocode in http://en.wikipedia.org/wiki/Mersenne_twister

#ifndef RANDGENMersenneTwister_HPP_
#define RANDGENMersenneTwister_HPP_

#include "../RandGen.hpp"
#include <limits.h>

namespace optframe {

class RandGenMersenneTwister : public RandGen
{
private:
   static const unsigned int GEN_STATE_LENGTH = 624;
   static const unsigned int MASK_1 = 0xffffffff; //4294967295
   static const unsigned int MASK_2 = 0x80000000; //2147483648
   static const unsigned int MASK_3 = 0x7fffffff; //2147483647

   static const unsigned int CONSTANT_1 = 0x6c078965; //1812433253
   static const unsigned int CONSTANT_2 = 0x9d2c5680; //2636928640
   static const unsigned int CONSTANT_3 = 0xefc60000; //4022730752
   static const unsigned int CONSTANT_4 = 0x9908b0df; //2567483615
   unsigned int MT[GEN_STATE_LENGTH];
   int index;

   void GenNum()
   {
      int i;
      unsigned int y;
      for (i = 0; i < ((int)GEN_STATE_LENGTH); i++) {
         y = (MT[i] ^ MASK_2) + (MT[(i + 1) % GEN_STATE_LENGTH] ^ MASK_3);
         MT[i] = MT[(i + 397) % GEN_STATE_LENGTH] ^ (y >> 1);
         if ((y % 2) == 1) {
            MT[i] = MT[i] ^ CONSTANT_4;
         }
      }
   }

   unsigned mt_rand()
   {
      unsigned int y;

      if (index == 0)
         GenNum();

      y = MT[index];
      y = y ^ (y >> 11);
      y = y ^ ((y << 7) & CONSTANT_2);
      y = y ^ ((y << 15) & CONSTANT_3);
      y = y ^ (y >> 18);

      index = (index + 1) % GEN_STATE_LENGTH;

      return y;
   }

public:
   RandGenMersenneTwister()
     : RandGen()
   {
      initialize();
   }

   RandGenMersenneTwister(unsigned seed)
     : RandGen(seed)
   {
      this->seed = seed; // override inheritance
      initialize();
   }

   using RandGen::rand;

   // initialize random number generation
   void initialize() override
   {
      int i;

      index = 0;
      MT[0] = seed;
      for (i = 1; i < ((int)GEN_STATE_LENGTH); i++) {
         MT[i] = ((CONSTANT_1 * (MT[i - 1] ^ (MT[i - 1] >> 30))) + i) & MASK_1;
      }
   }

   // randomized number between non-negatives [i, j]
   virtual unsigned rand(unsigned i, unsigned j) override
   {
      return this->rand(j - i + 1) + i;
   }

   // generate random number
   int rand() override
   {
      return mt_rand();
   }

   // generate random number between 0 and n-1
   unsigned rand(unsigned n) override
   {
      if (n < 0)
         n *= (-1);

      // TODO: https://ericlippert.com/2013/12/16/how-much-bias-is-introduced-by-the-remainder-technique/

      return mt_rand() % n;
   }

   // random uniform between [0,1)
   double rand01() override
   {
      return (double)mt_rand() / UINT_MAX;
   }
};

} // namespace optframe

#endif /* RANDGENMersenneTwister_HPP_ */
