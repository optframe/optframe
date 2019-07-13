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

class HFMMultiEvaluator: public MultiEvaluator<RepHFM, OPTFRAME_DEFAULT_ADS>
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


	MultiEvaluation evaluate(const RepHFM& r, const OPTFRAME_DEFAULT_ADS* ads)
	{
		MultiEvaluation nev;

		vector<double>* foIndicator = evalEFP.evaluateAll(r, ALL_EVALUATIONS);

		//It has been verified that most part of INDEX minimizes Square Errors and are strongly correlated
		//Instead of designed MAPE_INV
		nev.addEvaluation(EvaluationEFP(foIndicator->at(MAPE_INDEX)));
		nev.addEvaluation(EvaluationEFP(foIndicator->at(MAPE_INV_INDEX)));
//		nev.addEvaluation(EvaluationEFP(foIndicator->at(SMAPE_INDEX)));
//		nev.addEvaluation(EvaluationEFP(foIndicator->at(RMSE_INDEX)));
//		nev.addEvaluation(EvaluationEFP(foIndicator->at(WMAPE_INDEX)));
//		nev.addEvaluation(EvaluationEFP(foIndicator->at(MMAPE_INDEX)));

		delete foIndicator;
		return nev;
	}

	virtual void reevaluateMEV(MultiEvaluation& mev, const RepHFM& r, const OPTFRAME_DEFAULT_ADS* ads)
	{
		mev = evaluate(r,ads);
	}

	void addEvaluator(Evaluator<RepHFM, OPTFRAME_DEFAULT_ADS>& ev)
	{
		cout<<"I should not add anyone! HFM MEV"<<endl;
		getchar();
	}

	unsigned size() const
	{
		return 5;
	}

	bool betterThan(const Evaluation& ev1, const Evaluation& ev2, int index)
	{
		return evalEFP.betterThan(ev1, ev2);
	}

	bool equals(const Evaluation& ev1, const Evaluation& ev2, int index)
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

}

#endif /*HFM_MULTI_EVALUATOR_HPP_*/
