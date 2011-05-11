// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef OPTFRAME_MULTIOBJECTIVEEVALUATOR_HPP_
#define OPTFRAME_MULTIOBJECTIVEEVALUATOR_HPP_

#include "Evaluator.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"

#include <iostream>

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class MultiObjectiveEvaluator: public Evaluator<R, M>
{
protected:
	vector<Evaluator<R, M>*> partialEvaluators;
public:

	using Evaluator<R, M>::evaluate; // prevents name hiding

	MultiObjectiveEvaluator(Evaluator<R, M>& e)
	{
		partialEvaluators.push_back(&e);
	}

	virtual ~MultiObjectiveEvaluator()
	{
	}

	void add(Evaluator<R, M>& e)
	{
		partialEvaluators.push_back(&e);
	}

	virtual Evaluation<M>& evaluate(const R& r)
	{
		double objFunction = 0;
		double infMeasure = 0;

		Evaluation<M>& e = partialEvaluators.at(0)->evaluate(r);

		objFunction += e.getObjFunction();
		infMeasure += e.getInfMeasure();

		for (unsigned i = 1; i < partialEvaluators.size(); i++)
		{
			partialEvaluators.at(i)->evaluate(e, r);
			objFunction += e.getObjFunction();
			infMeasure += e.getInfMeasure();
		}

		e.setObjFunction(objFunction);
		e.setInfMeasure(infMeasure);

		return e;
	}

	virtual void evaluate(Evaluation<M>& e, const R& r)
	{
		double objFunction = 0;
		double infMeasure = 0;

		for (unsigned i = 0; i < partialEvaluators.size(); i++)
		{
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
};

#endif /*OPTFRAME_MULTIOBJECTIVEEVALUATOR_HPP_*/
