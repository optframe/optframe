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

/*
 * ParetoDominanceSWeak.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_PARETODOMINANCE_WEAK_HPP_
#define OPTFRAME_PARETODOMINANCE_WEAK_HPP_

#include "MultiEvaluation.hpp"
#include "ParetoDominance.hpp"

#include <cmath>
#include <iostream>

using namespace std;

namespace optframe {

template<XSolution S, XEvaluation XEv = Evaluation<>, XEvaluation XMEv = MultiEvaluation<>>
class ParetoDominanceWeak : public ParetoDominance<S, XEv, XMEv>
{
public:
   using ParetoDominance<S, XEv, XMEv>::dominates;

   ParetoDominanceWeak(MultiEvaluator<S, XEv, XMEv>& _mev)
     : ParetoDominance<S, XEv, XMEv>(_mev)
   {
   }

   virtual ~ParetoDominanceWeak()
   {
   }

   virtual bool dominates(const MultiEvaluation<>& mev1, const MultiEvaluation<>& mev2)
   {
      pair<int, int> betterEquals = ParetoDominance<S, XEv, XMEv>::checkDominates(mev1, mev2);
      int better = betterEquals.first;
      int equals = betterEquals.second;

      return (((unsigned)better + equals == mev1.size()));
   }
};

}

#endif /*OPTFRAME_PARETODOMINANCE_WEAK_HPP_*/
