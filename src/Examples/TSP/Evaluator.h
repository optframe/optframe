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

#ifndef TSP_EVALUATOR_HPP_
#define TSP_EVALUATOR_HPP_

//#include <cmath>
#include <math.h>
#include <stdlib.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Evaluation.h"
#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSPSwap.hpp"

#define TSP_EPSILON 0.0001

namespace TSP {

class TSPEvaluator : public Evaluator<SolutionTSP>
{
private:
   ProblemInstance* pI;

public:
   bool Minimizing;

   vector<int> solutions;
   vector<long long> solNSTotal;
   vector<long long> solNSImp;
   int solMin;
   int solMax;

   bool doStats;

   //using Evaluator<SolutionTSP>::evaluate; // prevents name hiding

   TSPEvaluator(ProblemInstance* pI)
     : Evaluator<SolutionTSP>(true) // ALLOW COSTS!
   {
      Minimizing = true;
      this->pI = pI;
      solutions = vector<int>(50000, 0);
      solMax = 0;
      solMin = 2000000000;
      solNSTotal = vector<long long>(50000, 0);
      solNSImp = solNSTotal;

      doStats = true;
   }

   bool verify(const RepTSP& r)
   {
      assert(int(r.size()) == pI->n);
      vector<bool> count(pI->n, false);
      for (int i = 0; i < pI->n; i++)
         count[r[i]] = true;
      for (int i = 0; i < pI->n; i++)
         assert(count[i]);
      return true;
   }

   Evaluation<> evaluate(const SolutionTSP& s) override
   {
      const RepTSP& r = s.getR();
      double fo = 0; // Evaluation<> Function Value

      for (int i = 0; i < ((int)r.size()) - 1; i++) {
         int j = r.at(i);
         int z = r.at(i + 1);

         double val = (*pI->dist)(j, z);
         fo += val;
      }

      int k = r.at(((int)r.size()) - 1);
      int l = r.at(0);

      double val = (*pI->dist)(k, l);
      fo += val;

      //fo = int(fo); // cast to int
      /*
        if (doStats)
        {
            int ifo = int(fo);

            if (ifo >= int(solutions.size()))
            {
                cout << "ERROR!!!! FO IS BIGGER THAN SOLUTION LIMIT!" << ifo << endl;
                exit(1);
            }
            solutions[ifo]++;

            if (ifo < solMin)
                solMin = ifo;
            if (ifo > solMax)
                solMax = ifo;

            // explore neighborhood
            int total = 0;
            int impSols = 0;

            exploreNeighborhood(r, total, impSols);
            if (total == 0)
            {
                cout << "ERRO! no neighbors!!" << endl;
                cout << r << endl;
                exit(1);
            }

            solNSTotal[ifo] += total;
            solNSImp[ifo] += impSols;
        }
        */

      return Evaluation<>(fo);
   }

   void exploreNeighborhood(RepTSP r, int& totalNeigh, int& improvingSols)
   {
      NSSeqTSPSwap<int> tspswap;

      //int ads;
      SolutionTSP s(r); // TODO: think
      NSIterator<SolutionTSP>& it = *tspswap.getIterator(s);
      it.first();

      //cout << "got iterator: " << it.toString() << endl;

      CopySolution<RepTSP> base(r, nullptr);
      while (!it.isDone()) {
         //cout << "will get move" << endl;
         totalNeigh++;
         Move<SolutionTSP>& m = *it.current();
         //m.print();
         SolutionTSP s2(r); // TODO: think
         if (m.canBeApplied(s2)) {
            doStats = false;
            //Evaluation<>* e = nullptr; // dummy // TODO:
            EvaluationTSP e; // dummy // TODO:
            pair<SolutionTSP, EvaluationTSP> pse(base, e);
            
            //MoveCost<>& mc = *this->moveCost(m, pse);
            op<Evaluation<>> mc = this->moveCost(m, pse);
            doStats = true;
            //if (mc.getObjFunctionCost() < 0)
            if (mc->evaluation() < 0)
               improvingSols++;
            //delete &mc;
         }
         delete &m;
         it.next();
      }
      //cout << "finished explore" << endl;
   }

   /*
      // TODO: remove this? // maybe only operate on XEv space?
    virtual bool betterThan(evtype f1, evtype f2)
    {
        if (Minimizing)
            return (f1 < (f2 - TSP_EPSILON));
        else
            return (f1 > (f2 + TSP_EPSILON));
    }
*/

   // override from Direction
   virtual inline bool betterThan(const Evaluation<double>& e1, const Evaluation<double>& e2) override
   {
      //cout << "betterThan!" << endl;
      //e1.print();
      //e2.print();
      //
      if (isMinimization())
         //return ::fabs(e2.evaluation() - e1.evaluation()) >= 0.00001;
         return (e2.evaluation() - e1.evaluation()) > 0.00001;
      else
         //return ::fabs(e1.evaluation() - e2.evaluation()) >= 0.00001;
         return (e1.evaluation() - e2.evaluation()) > 0.00001;
   }

   // override from Direction
   virtual bool isMinimization() const override
   {
      return Minimizing;
   }

   virtual string id() const
   {
      string pai = Evaluator<SolutionTSP>::idComponent();
      pai.append(":TSPEvaluator");
      return pai;
   }
};
}

#endif /*TSP_EVALUATOR_HPP_*/
