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

#ifndef OPTFRAME_IILS_HPP_
#define OPTFRAME_IILS_HPP_

#include <math.h>
#include <vector>

#include "../../Evaluator.hpp"
#include "../../SingleObjSearch.hpp"

namespace optframe {

template<class H, class R, class ADS = OPTFRAME_DEFAULT_ADS>
class IntensifiedIteratedLocalSearch : public SingleObjSearch<XES>
{
protected:
   Evaluator<S>& evaluator;
   InitialSearch<XES, XEv>& constructive;

public:
   IntensifiedIteratedLocalSearch(Evaluator<S>& _evaluator, InitialSearch<XES, XEv>& _constructive)
     : evaluator(_evaluator)
     , constructive(_constructive)
   {
   }

   virtual ~IntensifiedIteratedLocalSearch()
   {
   }

   Evaluator<S>& getEvaluator()
   {
      return evaluator;
   }

   virtual H& initializeHistory() = 0;

   virtual void localSearch(Solution<R, ADS>& s, Evaluation<>& e, double timelimit, double target_f) = 0;

   virtual void intensification(Solution<R, ADS>& s, Evaluation<>& e, double timelimit, double target_f, H& history) = 0;

   virtual void perturbation(Solution<R, ADS>& s, Evaluation<>& e, double timelimit, double target_f, H& history) = 0;

   virtual Solution<R, ADS>& acceptanceCriterion(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2, H& history) = 0;

   virtual bool terminationCondition(H& history) = 0;

   pair<Solution<R, ADS>&, Evaluation<>&>* search(double timelimit = 100000000, double target_f = 0, const Solution<R, ADS>* _s = nullptr, const Evaluation<>* _e = nullptr)
   {
      cout << "IILS search(" << target_f << "," << timelimit << ")" << endl;

      Timer tnow;

      Solution<R, ADS>& s = constructive.generateSolution();
      Evaluation<>& e = evaluator.evaluate(s);

      H* history = &initializeHistory();

      // 's0' <- GenerateSolution
      // 's*' <- localSearch 's'

      localSearch(s, e, (timelimit - (tnow.now())), target_f);

      Solution<R, ADS>* sStar = &s.clone();
      Evaluation<>* eStar = &e.clone();

      cout << "IILS starts: ";
      e.print();

      do {
         Solution<R, ADS>* s1 = &sStar->clone();
         Evaluation<>* e1 = &eStar->clone();

         perturbation(*s1, *e1, (timelimit - (tnow.now())), target_f, *history);

         localSearch(*s1, *e1, (timelimit - (tnow.now())), target_f);

         intensification(*s1, *e1, (timelimit - (tnow.now())), target_f, *history);

         Solution<R, ADS>* s2 = s1;
         Evaluation<>* e2 = e1;

         Solution<R, ADS>* sStar1 = &acceptanceCriterion(*sStar, *s2, *history);

         delete sStar;
         delete eStar;
         delete s2;
         delete e2;

         sStar = sStar1;
         eStar = &evaluator.evaluate(*sStar);

      } while (evaluator.betterThan(target_f, eStar->evaluation()) && !terminationCondition(*history) && ((tnow.now()) < timelimit));

      e = *eStar;
      s = *sStar;

      delete eStar;
      delete sStar;

      delete history;

      return new pair<Solution<R, ADS>&, Evaluation<>&>(s, e);
   }

   virtual string id() const
   {
      return idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearch<XES>::idComponent() << "IILS:";
      return ss.str();
   }
};

}

#endif /*OPTFRAME_IILS_HPP_*/
