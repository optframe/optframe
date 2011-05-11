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

#ifndef OPTFRAME_BASICILS_HPP_
#define OPTFRAME_BASICILS_HPP_

#include <math.h>
#include <vector>

#include "IteratedLocalSearch.hpp"
#include "BasicILSPerturbation.hpp"

typedef int BasicHistory;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class BasicIteratedLocalSearch: public IteratedLocalSearch<BasicHistory, R, M>
{
protected:
	Heuristic<R, M>& h;
	BasicILSPerturbation<R, M>& p;
	int iterMax;

public:

	BasicIteratedLocalSearch(Evaluator<R, M>& e, Heuristic<R, M>& _h, BasicILSPerturbation<R, M>& _p, int _iterMax) :
		IteratedLocalSearch<BasicHistory, R, M> (e), h(_h), p(_p), iterMax(_iterMax)
	{
	}

	virtual BasicHistory& initializeHistory()
	{
		int& iter = * new int;
		iter = 0;

		return iter;
	}

	virtual void localSearch(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		h.exec(s, e, timelimit, target_f);
	}

	virtual void perturbation(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f, BasicHistory& history)
	{
		int iter = history;

		p.perturb(s, e, timelimit, target_f);

		// Incrementa a iteracao
		iter++;

		// Atualiza o historico
		history = iter;
	}

	virtual Solution<R>& acceptanceCriterion(const Solution<R>& s1, const Solution<R>& s2, BasicHistory& history)
	{
		if (IteratedLocalSearch<BasicHistory, R, M>::evaluator.betterThan(s2, s1))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			Evaluation<M>& e = IteratedLocalSearch<BasicHistory, R, M>::evaluator.evaluate(s2);
			cout << "Best fo: " << e.evaluation();
			cout << " on [iter " << history << "]" << endl;
			delete &e;

			// =======================
			//  Atualiza o historico
			// =======================
			// iter = 0
			history = 0;

			// =======================
			//    Retorna s2
			// =======================
			return s2.clone();
		}
		else
			return s1.clone();
	}

	virtual bool terminationCondition(BasicHistory& history)
	{
		int iter = history;

		return (iter >= iterMax);
	}
};

#endif /*OPTFRAME_BASICILS_HPP_*/
