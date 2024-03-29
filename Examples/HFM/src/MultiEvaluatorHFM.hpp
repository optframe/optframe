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

#ifndef HFM_MULTI_EVALUATOR_HPP_
#define HFM_MULTI_EVALUATOR_HPP_

#include <OptFrame/MultiEvaluator.hpp>
#include <iostream>

using namespace std;
using namespace scannerpp;

namespace HFM {

class HFMMultiEvaluator : public MultiEvaluator<SolutionHFM, EvaluationHFM, MultiEvaluationHFM>
{
   sref<HFMEvaluator> evalEFP;

public:
   HFMMultiEvaluator(sref<HFMEvaluator> _evalEFP)
     : evalEFP(_evalEFP)
   {
   }

   ~HFMMultiEvaluator()
   {
   }

   MultiEvaluation<> evaluate(const SolutionHFM& s) override
   {
      const RepHFM& r = s.getR();
      MultiEvaluation<> nev;

      vector<double>* foIndicator = evalEFP->evaluateAll(r, ALL_EVALUATIONS);

      //It has been verified that most part of INDEX minimizes Square Errors and are strongly correlated
      //Instead of designed MAPE_INV
      nev.addEvaluation(EvaluationHFM(foIndicator->at(MAPE_INDEX)));
      nev.addEvaluation(EvaluationHFM(foIndicator->at(MAPE_INV_INDEX)));
      //		nev.addEvaluation(EvaluationHFM(foIndicator->at(SMAPE_INDEX)));
      //		nev.addEvaluation(EvaluationHFM(foIndicator->at(RMSE_INDEX)));
      //		nev.addEvaluation(EvaluationHFM(foIndicator->at(WMAPE_INDEX)));
      //		nev.addEvaluation(EvaluationHFM(foIndicator->at(MMAPE_INDEX)));

      delete foIndicator;
      return nev;
   }

   virtual void reevaluate(pair<SolutionHFM, MultiEvaluation<>>& smev) override
   {
      smev.second = evaluate(smev.first);
   }

   void addEvaluator(Evaluator<SolutionHFM, Evaluation<>>& ev)
   {
      cout << "I should not add anyone! HFM MEV" << endl;
      getchar();
   }

   unsigned size() const
   {
      return 5;
   }

   bool betterThan(const Evaluation<>& ev1, const Evaluation<>& ev2, int index) override
   {
      return evalEFP->betterThan(ev1, ev2);
   }

   bool equals(const Evaluation<>& ev1, const Evaluation<>& ev2, int index) override
   {
      return evalEFP->equals(ev1, ev2);
   }

protected:
   static string idComponent()
   {
      stringstream ss;
      ss << MultiDirection::idComponent() << ":MultiEvaluator:HFM";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

static_assert(std::is_base_of<MultiEvaluator<SolutionHFM, EvaluationHFM, MultiEvaluationHFM>, HFMMultiEvaluator>::value, "not inherited from MultiEvaluator");
//static_assert(std::is_base_of<GeneralEvaluator< EMSolutionHFM  >, HFMMultiEvaluator >::value,  "not inherited from GeneralEvaluator");

} // namespace hfm

#endif /*HFM_MULTI_EVALUATOR_HPP_*/
