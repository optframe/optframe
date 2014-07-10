// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#define TSP_EPSILON 0.0001

namespace TSP
{

class TSPEvaluator: public Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>
{
private:
	ProblemInstance* pI;

public:

	using Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::evaluate; // prevents name hiding

	TSPEvaluator(ProblemInstance* pI) :
		Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>(true) // ALLOW COSTS!
	{
		this->pI = pI;
	}

	Evaluation<MemTSP>& evaluate(const RepTSP& r)
	{
		double fo = 0; // Evaluation Function Value

		for (int i = 0; i < ((int)r.size()) - 1; i++)
		{
			int j = r.at(i);
			int z = r.at(i + 1);

			double val = (*pI->dist)(j, z);
			fo += val;
		}

		int k = r.at(((int)r.size()) - 1);
		int l = r.at(0);

		double val = (*pI->dist)(k, l);
		fo += val;

		MemTSP mem(false, 0);

		return *new Evaluation<MemTSP> (fo, mem);
	}


	void evaluate(Evaluation<MemTSP>& e, const RepTSP& r, const OPTFRAME_DEFAULT_ADS& ads)
	{
		if(!e.getDS().first) // use slower
		{
			Evaluation<MemTSP>& e1 = evaluate(r, ads);
			e.setDS(e1.getDS());
			e.setObjFunction(e1.getObjFunction());
			e.setInfMeasure(e1.getInfMeasure());
			delete &e1;
			return;
		}

		e.setObjFunction(e.getObjFunction() + e.getDS().second);
		e.setDS(make_pair(false, 0));
	}


	virtual bool betterThan(double f1, double f2)
	{
		return (f1 < (f2 - TSP_EPSILON));
	}

	virtual bool isMinimization() const
	{
		return true;
	}

	virtual string id() const
	{
		string pai = Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::idComponent();
		pai.append(":TSPEvaluator");
		return pai;
	}

};

}

#endif /*TSP_EVALUATOR_HPP_*/
