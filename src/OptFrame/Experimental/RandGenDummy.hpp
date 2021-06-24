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

#ifndef RANDGENDummy_HPP_
#define RANDGENDummy_HPP_

#include "../../RandGen.hpp"

class RandGenDummy : public RandGen
{
private:
   unsigned int i;
   int inc;

   unsigned int d_rand()
   {
      if (!RandGen::init) {
         initialize();
         RandGen::init = true;
      }

      i += inc;

      return i;
   }

public:
   RandGenDummy()
     : RandGen()
   {
      inc = 1;
   }

   RandGenDummy(long seed, int _inc = 1)
     : inc(_inc)
     , RandGen(seed)
   {
   }

   using RandGen::rand;

   // initialize random number generation
   void initialize()
   {
      i = RandGen::seed;
   }

   // generate random number
   int rand()
   {
      return d_rand();
   }

   // generate random number between 0 and n-1
   int rand(int n)
   {
      return d_rand() % n;
   }

   // random uniform between [0,1)
   double rand01()
   {
      return rand(1000) / 1000.0;
   }

   // random gaussian mean 0.0 stdev 1.0
   virtual double randG()
   {
      float x, w, y;
      float x_, _x, w_; // dummy version!
      do {
         x = 2.0 * rand01() - 1.0;
         w = x * x + x * x;

         // ==============================
         // to avoid infinite loop
         // create another crazy number!
         // ==============================
         x_ = (rand(200) / 100.0) - 1.0;
         _x = ((200 - rand(200)) / 100.0) - 1.0;
         w_ = (x_ * x_ + _x * _x);
         if ((w >= 1.0) && (w_ < w)) {
            w = w_;
            x = _x; // or x_
         }
         // ==============================

      } while (w >= 1.0);

      w = sqrt((-2.0 * log(w)) / w);
      y = x * w;

      return y;
   }
};

#endif /* RANDGENDummy_HPP_ */
