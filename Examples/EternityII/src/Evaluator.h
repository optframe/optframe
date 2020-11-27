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

#ifndef EtII_EVALUATOR_HPP_
#define EtII_EVALUATOR_HPP_

//#include <cmath>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>

#include "Evaluation.h"
#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

using namespace optframe;

#define EPSILON_EtII 0.0001

extern int numEvs;

namespace EtII {

class EtIIEvaluator : public Evaluator<SolutionEtII, EvaluationEtII>
{
private:
   ProblemInstance& pEtII;

public:
   using Evaluator<SolutionEtII, EvaluationEtII>::evaluate;

   EtIIEvaluator(ProblemInstance& _pEtII)
     : Evaluator<SolutionEtII, EvaluationEtII>(true)
     , pEtII(_pEtII) // DISALLOW COSTS (DEFAULT)
   {
      // Put the rest of your code here
   }

   Evaluation<> evaluate(const SolutionEtII& s) override
   {
      const RepEtII& rep = s.getR();
      //counting evaluations.
      numEvs++;
      //cout<<"##### Number of evaluates: "<<numEvs<<endl;

      int fo = 0; // Evaluation<> Function Value

      // horizontal
      for (unsigned int i = 0; i < rep.getNumRows(); i++)
         for (unsigned int j = 0; j < rep.getNumCols() - 1; j++)
            if (rep(i, j).right == rep(i, j + 1).left)
               fo++;

      // vertical
      for (unsigned int j = 0; j < rep.getNumCols(); j++)
         for (unsigned int i = 0; i < rep.getNumRows() - 1; i++)
            if (rep(i, j).down == rep(i + 1, j).up)
               fo++;

      return Evaluation<>(fo);
   }

   virtual bool betterThan(double f1, double f2)
   {
      return (f1 > (f2 - EPSILON_EtII));
   }

   virtual bool isMinimization() const
   {
      return false;
   }
};
}

#endif /*EtII_EVALUATOR_HPP_*/
