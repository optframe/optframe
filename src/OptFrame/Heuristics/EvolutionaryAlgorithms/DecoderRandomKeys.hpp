// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_DECODER_RANDOM_KEYS_HPP_
#define OPTFRAME_DECODER_RANDOM_KEYS_HPP_

#include "../../Evaluation.hpp"
#include "../../Evaluator.hpp"
#include "../../Solution.hpp"
#include "../../Solutions/CopySolution.hpp"

namespace optframe {

typedef vector<double> random_keys;

// Representation R is NOT random_keys... it's something else, related to original problem
template<Representation R, XSolution S = RSolution<R>, XEvaluation XEv = Evaluation<>>
class DecoderRandomKeys
{
public:
   virtual ~DecoderRandomKeys()
   {
   }

   virtual pair<XEv, S*> decode(const random_keys& rk) = 0;

   virtual bool isMinimization() const = 0;
};

// ========================================================
// Basic implementation to use class Evaluator<random_keys>
// ========================================================

// what is this class??
///template<Representation R, XSolution S = RSolution<R>, XEvaluation XEv = Evaluation<>>
// probably, a decoder for a solution typed 'random_keys'... strange!
// maybe its meant to be generic... and simply use a delegated 'Evaluator'....
template<Representation R, XSolution S = RSolution<R>, XEvaluation XEv = Evaluation<>>
class DecoderRandomKeysEvaluator : public DecoderRandomKeys<R, S, XEv>
{
public:

   //using RKEvaluator = Evaluator<random_keys, OPTFRAME_DEFAULT_ADS, CopySolution<random_keys,OPTFRAME_DEFAULT_ADS>;

   Evaluator<R>& evaluator;

   DecoderRandomKeysEvaluator(Evaluator<R>& _evaluator)
     : evaluator(_evaluator)
   {
   }

   virtual ~DecoderRandomKeysEvaluator()
   {
   }

   virtual pair<XEv, S*> decode(const R& rk) override
   {
      return pair<XEv, S*>(evaluator.evaluate(rk, nullptr), nullptr);
   }

   virtual bool isMinimization() const
   {
      return evaluator.isMinimization();
   }
};


// application for R = permutation of int (vector<int>)
template<XRSolution<vector<int>> XRS, XEvaluation XEv = Evaluation<>>
class EvaluatorPermutationRandomKeys : public DecoderRandomKeys<vector<int>, XRS>
{
public:
   Evaluator<XRS>& ev; // evaluator for permutation
   int a, b;                   // decode in interval [a,b]

   EvaluatorPermutationRandomKeys(Evaluator<XRS>& _ev, int _a, int _b)
     : ev(_ev)
     , a(_a)
     , b(_b)
   {
      assert(a <= b);
   }

   virtual pair<XEv, XRS*> decode(const random_keys& rk) override
   {
      int sz = b - a + 1;
      vector<pair<double, int>> v(sz);
      int k = 0;
      for (int i = a; i <= b; i++, k++)
         v[k] = pair<double, int>(rk[i], k);

      sort(v.begin(), v.end(), [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
         return i.first < j.first;
      });

      vector<int> p(v.size());
      for (unsigned i = 0; i < v.size(); i++)
         p[i] = v[i].second;

      XRS sevp(p);
      XEv e = ev.evaluate(sevp);

      // you have the option to actually return a Solution<vector<int>> for post-decoding purposes
      return pair<XEv, XRS*>(e, new XRS(p));
   }

   virtual bool isMinimization() const
   {
      return ev.isMinimization();
   }
};

// implementation of decoder for subset function (vector<bool>)
class EvaluatorSubsetRandomKeys : public DecoderRandomKeys<vector<bool>, RSolution<vector<bool>>>
{
public:
   Evaluator<RSolution<vector<bool>>>& ev; // evaluator for permutation
   int a, b;                    // decode in interval [a,b]
   double limit;                // limit to decide membership (default=0.5)

   EvaluatorSubsetRandomKeys(Evaluator<RSolution<vector<bool>>>& _ev, int _a, int _b, double _limit = 0.5)
     : ev(_ev)
     , a(_a)
     , b(_b)
     , limit(_limit)
   {
      assert(a <= b);
   }

   virtual pair<Evaluation<>, RSolution<vector<bool>>*> decode(const random_keys& rk) override
   {
      vector<bool> v(b - a + 1);
      for (unsigned i = 0; i < v.size(); i++)
         v[i] = rk[i] >= limit;

      RSolution<vector<bool>> sev(v);
      Evaluation<> e = ev.evaluate(sev);

      // you have the option to actually return a Solution<vector<bool>> for post-decoding purposes
      return pair<Evaluation<>&, RSolution<vector<bool>>*>(e, nullptr);
   }

   virtual bool isMinimization() const
   {
      return ev.isMinimization();
   }
};
}

#endif /*OPTFRAME_DECODER_RANDOM_KEYS_HPP_*/
