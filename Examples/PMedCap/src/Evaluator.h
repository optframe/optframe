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

#ifndef PCAP_EVALUATOR_HPP_
#define PCAP_EVALUATOR_HPP_

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

#define EPSILON_PCAP 0.0001

class PCAPEvaluator : public Evaluator<SolutionPCAP>
{
private:
   PCAPProblemInstance& pPCAP;

   // Your private vars

public:
   //using Evaluator<ESolutionPCAP>::evaluate;

   PCAPEvaluator(PCAPProblemInstance& _pPCAP)
     : // If necessary, add more parameters
     pPCAP(_pPCAP)
   {
      // Put the rest of your code here
   }

   Evaluation<> evaluate(const SolutionPCAP& s) override
   {
      const RepPCAP& rep = s.getR();
      // 'rep' is the representation of the solution

      double fo = 0; // Evaluation<> Function Value
      double fo_Inv = 0;

      vector<int> utilizacao;
      for (unsigned int i = 0; i < rep.first.size(); i++)
         utilizacao.push_back(0);

      for (unsigned int i = 0; i < rep.second.size(); i++) {
         int med = rep.first[rep.second[i]];
         int indiceMed = rep.second[i];

         double x = pPCAP.vecCidades[i].x;
         double y = pPCAP.vecCidades[i].y;
         int d = pPCAP.vecCidades[i].demanda;

         utilizacao[indiceMed] += d;
         if (utilizacao[indiceMed] > pPCAP.vecCidades[med].capacidade)
            fo_Inv += utilizacao[indiceMed] - pPCAP.vecCidades[med].capacidade;

         double xMed = pPCAP.vecCidades[med].x;
         double yMed = pPCAP.vecCidades[med].y;

         fo += sqrt(pow((x - xMed), 2) + pow((y - yMed), 2));
         //cout << "i = " << i << " fo = " << fo << endl;
         //getchar();
      }

      double fo_medAlocada = 0; //Garante que a mediana eh atendida por ela propria

      for (unsigned int i = 0; i < rep.first.size(); i++) {
         int med = rep.first[i];
         if (rep.second[med] != (int)i)
            fo_medAlocada += 100000;
      }

      fo_Inv = fo_Inv * 10000 + fo_medAlocada;

      return Evaluation<>(fo, fo_Inv);
   }

   virtual bool betterThan(double f1, double f2)
   {
      // MINIMIZATION
      return (f1 < (f2 - EPSILON_PCAP));
   }

   virtual bool isMinimization() const
   {
      return true;
   }
};

static_assert(std::is_base_of<Evaluator<SolutionPCAP>, PCAPEvaluator>::value, "not inherited from Evaluator");
static_assert(std::is_base_of<GeneralEvaluator<ESolutionPCAP>, PCAPEvaluator>::value, "not inherited from GeneralEvaluator");

#endif /*PCAP_EVALUATOR_HPP_*/
