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

#ifndef OPTFRAME_MULTIOBJECTIVEEVALUATOR_HPP_
#define OPTFRAME_MULTIOBJECTIVEEVALUATOR_HPP_

#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "Move.hpp"

#include <iostream>

using namespace std;

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiObjectiveEvaluator : public Evaluator<R, ADS, DS>
{
protected:
   vector<Evaluator<R, ADS, DS>*> partialEvaluators;

public:
   using Evaluator<R, ADS, DS>::evaluate; // prevents name hiding

   MultiObjectiveEvaluator(Evaluator<R, ADS, DS>& e)
   {
      partialEvaluators.push_back(&e);
   }

   virtual ~MultiObjectiveEvaluator()
   {
   }

   void add(Evaluator<R, ADS, DS>& e)
   {
      partialEvaluators.push_back(&e);
   }

   virtual Evaluation<DS>& evaluate(const R& r)
   {
      double objFunction = 0;
      double infMeasure = 0;

      Evaluation<DS>& e = partialEvaluators.at(0)->evaluate(r);

      objFunction += e.getObjFunction();
      infMeasure += e.getInfMeasure();

      for (unsigned i = 1; i < partialEvaluators.size(); i++) {
         partialEvaluators.at(i)->evaluate(e, r);
         objFunction += e.getObjFunction();
         infMeasure += e.getInfMeasure();
      }

      e.setObjFunction(objFunction);
      e.setInfMeasure(infMeasure);

      return e;
   }

   virtual void evaluate(Evaluation<DS>& e, const R& r)
   {
      double objFunction = 0;
      double infMeasure = 0;

      for (unsigned i = 0; i < partialEvaluators.size(); i++) {
         partialEvaluators[i]->evaluate(e, r);
         objFunction += e.getObjFunction();
         infMeasure += e.getInfMeasure();
      }

      e.setObjFunction(objFunction);
      e.setInfMeasure(infMeasure);
   }

   virtual bool betterThan(double a, double b)
   {
      return partialEvaluators[0]->betterThan(a, b);
   }

   static string idComponent()
   {
      return "OptFrame:moev";
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_MULTIOBJECTIVEEVALUATOR_HPP_*/
