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

// XRepresentation R is NOT random_keys... it's something else, related to original problem
//template<XRepresentation R, XRSolution<R> XRS = RSolution<R>, XEvaluation XEv = Evaluation<>>
// XRS is not good to be default, as it must come from outside, and be compatible
template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv = Evaluation<>>
class DecoderRandomKeys
{
public:
   virtual ~DecoderRandomKeys()
   {
   }

   // returns evaluation and, optionally the constructed solution.
   // Flag 'needsSolution' indicates that solution is ABSOLUTELY necessary...
   virtual pair<XEv, op<XRS>> decode(const random_keys& rk, bool needsSolution) = 0;

   virtual bool isMinimization() const = 0;
};

// ========================================================
// Basic implementation to use class Evaluator<random_keys>
// ========================================================

// transforms random_keys into R and use given evaluator<R> to generate output XEv
// XRS is not good to be default, as it must come from outside, and be compatible
template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv = Evaluation<>>
class DecoderRandomKeysEvaluator : public DecoderRandomKeys<R, XRS, XEv>
{
public:

   //using RKEvaluator = Evaluator<random_keys, OPTFRAME_DEFAULT_ADS, CopySolution<random_keys,OPTFRAME_DEFAULT_ADS>;

   Evaluator<XRS>& evaluator;

   DecoderRandomKeysEvaluator(Evaluator<XRS>& _evaluator)
     : evaluator(_evaluator)
   {
   }

   virtual ~DecoderRandomKeysEvaluator()
   {
   }

   virtual pair<XEv, op<XRS>> decode(const R& rk, bool needsSolution) override
   {
      return pair<XEv, op<XRS>>(evaluator.evaluate(rk, nullptr), nullptr);
   }

   virtual bool isMinimization() const
   {
      return evaluator.isMinimization();
   }
};


// transforms 'random_keys' into a XRS solution (with R=permutation), then use Evaluator<XRS> to generate output XEv
template<XRSolution<vector<int>> XRS, XEvaluation XEv = Evaluation<>>
class EvaluatorPermutationRandomKeys : public DecoderRandomKeys<vector<int>, XRS, XEv>
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

   virtual pair<XEv, op<XRS>> decode(const random_keys& rk, bool needsSolution) override
   {
      int sz = b - a + 1;
      vector<pair<double, int>> v(sz);
      int k = 0;
      for (int i = a; i <= b; i++, k++)
         v[k] = pair<double, int>(rk[i], k);

      sort(v.begin(), v.end(), [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
         return i.first < j.first;
      });

      // R = vector<int>
      vector<int> p(v.size());
      for (unsigned i = 0; i < v.size(); i++)
         p[i] = v[i].second;

      // XRS is user solution, based on 'vector<int>'      
      XRS sevp(p);
      XEv e = ev.evaluate(sevp);

      // you have the option to actually return a Solution<vector<int>> for post-decoding purposes
      return pair<XEv, op<XRS>>(e, make_optional(XRS(p)));
   }

   virtual bool isMinimization() const
   {
      return ev.isMinimization();
   }
};

// implementation of decoder for subset function (vector<bool>)
template<XRSolution<vector<bool>> XRS, XEvaluation XEv = Evaluation<>>
class EvaluatorSubsetRandomKeys : public DecoderRandomKeys<vector<bool>, XRS, XEv>
{
public:
   Evaluator<XRS>& ev; // evaluator for permutation
   int a, b;                    // decode in interval [a,b]
   double limit;                // limit to decide membership (default=0.5)

   EvaluatorSubsetRandomKeys(Evaluator<XRS>& _ev, int _a, int _b, double _limit = 0.5)
     : ev(_ev)
     , a(_a)
     , b(_b)
     , limit(_limit)
   {
      assert(a <= b);
   }

   virtual pair<Evaluation<>, op<XRS>> decode(const random_keys& rk, bool needsSolution) override
   {
      vector<bool> v(b - a + 1);
      for (unsigned i = 0; i < v.size(); i++)
         v[i] = rk[i] >= limit;

      XRS sev(v);
      Evaluation<> e = ev.evaluate(sev);

      // you have the option to actually return a Solution<vector<bool>> for post-decoding purposes
      return pair<Evaluation<>, op<XRS>>(e, make_optional(XRS(sev)));
   }

   virtual bool isMinimization() const
   {
      return ev.isMinimization();
   }
};
}

#endif /*OPTFRAME_DECODER_RANDOM_KEYS_HPP_*/
