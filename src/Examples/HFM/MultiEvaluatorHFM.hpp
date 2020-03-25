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

#ifndef HFM_MULTI_EVALUATOR_HPP_
#define HFM_MULTI_EVALUATOR_HPP_

#include <iostream>
#include "../../OptFrame/MultiEvaluator.hpp"

using namespace std;
using namespace scannerpp;

namespace HFM
{

class HFMMultiEvaluator: public MultiEvaluator<SolutionHFM, EvaluationHFM, MultiEvaluationHFM>
{
	HFMEvaluator& evalEFP;
public:

	HFMMultiEvaluator(HFMEvaluator& _evalEFP) :
			evalEFP(_evalEFP)
	{
	}

	~HFMMultiEvaluator()
	{

	}


	MultiEvaluation<> evaluate(const SolutionHFM& s) override
	{
      const RepHFM& r = s.getR();
		MultiEvaluation<> nev;

		vector<double>* foIndicator = evalEFP.evaluateAll(r, ALL_EVALUATIONS);

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

	void addEvaluator(Evaluator<SolutionHFM>& ev)
	{
		cout<<"I should not add anyone! HFM MEV"<<endl;
		getchar();
	}

	unsigned size() const
	{
		return 5;
	}

	bool betterThan(const Evaluation<>& ev1, const Evaluation<>& ev2, int index)
	{
		return evalEFP.betterThan(ev1, ev2);
	}

	bool equals(const Evaluation<>& ev1, const Evaluation<>& ev2, int index)
	{
		return evalEFP.equals(ev1, ev2);
	}

protected:

	static string idComponent()
	{
		stringstream ss;
		ss << MultiDirection::idComponent() << ":MultiEvaluator:HFM";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

static_assert(std::is_base_of<MultiEvaluator< SolutionHFM, EvaluationHFM, MultiEvaluationHFM >, HFMMultiEvaluator >::value,  "not inherited from MultiEvaluator");
//static_assert(std::is_base_of<GeneralEvaluator< EMSolutionHFM  >, HFMMultiEvaluator >::value,  "not inherited from GeneralEvaluator");

} // namespace hfm

#endif /*HFM_MULTI_EVALUATOR_HPP_*/
