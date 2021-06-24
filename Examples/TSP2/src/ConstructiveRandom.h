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

#ifndef TSP2_CONSTRUCTIVE_RANDOM_H_
#define TSP2_CONSTRUCTIVE_RANDOM_H_

#include "../../OptFrame/Constructive.hpp"
#include "../../OptFrame/RandGen.hpp"

#include "ProblemInstance.h"
#include "Representation.h"

using namespace optframe;
using namespace std;

namespace TSP2 {

class ConstructiveRandom : public Constructive<RepTSP>
{
private:
   ProblemInstance& pTSP;
   RandGen& rg;

public:
   ConstructiveRandom(ProblemInstance& _pTSP, RandGen& _rg)
     : pTSP(_pTSP)
     , rg(_rg)
   {
   }

   virtual ~ConstructiveRandom()
   {
   }

   Solution<RepTSP>& generateSolution()
   {
      RepTSP r(pTSP.n);

      for (unsigned int i = 0; i < r.size(); i++)
         r[i] = i;

      rg.shuffle(r); // shuffle elements of r

      return *new Solution<RepTSP>(r);
   }

   void print() const
   {
      cout << "Random Constructive" << endl;
   }
};

}

#endif /*TSP2_CONSTRUCTIVE_RANDOM_H_*/
